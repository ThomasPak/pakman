#include <iostream>
#include <memory>
#include <string>
#include <queue>

#include <assert.h>

#include <mpi.h>

#include "spdlog/spdlog.h"

#include "mpi/mpi_utils.h"
#include "mpi/mpi_common.h"
#include "controller/AbstractController.h"

#include "MPIMaster.h"

// Construct from pointer to program terminated flag
MPIMaster::MPIMaster(bool *p_program_terminated) :
    AbstractMaster(p_program_terminated),
    m_comm_size(get_mpi_comm_world_size()),
    m_map_manager_to_task(get_mpi_comm_world_size()),
    m_message_buffers(get_mpi_comm_world_size())
{
    // Initialize requests to MPI_REQUEST_NULL
    // and initialize idle managers
    for (int i = 0; i < m_comm_size; i++)
    {
        m_message_requests.push_back(MPI_REQUEST_NULL);
        m_signal_requests.push_back(MPI_REQUEST_NULL);
        m_idle_managers.insert(i);
    }
}

// Destroy MPI_Request objects
MPIMaster::~MPIMaster()
{
    // If MPI_Finalize has been called, nothing needs to be done
    int finalized = 0;
    MPI_Finalized(&finalized);

    if (finalized)
        return;

    // Else free any non-null requests
    for (int i = 0; i < m_comm_size; i++)
    {
        if (m_message_requests[i] != MPI_REQUEST_NULL)
            MPI_Request_free(&m_message_requests[i]);
        if (m_signal_requests[i] != MPI_REQUEST_NULL)
            MPI_Request_free(&m_signal_requests[i]);
    }
}

// Probe whether Master is active
bool MPIMaster::isActive() const
{
    return m_state != terminated;
}

// Iterate
void MPIMaster::iterate()
{
    // This function should never be called recursively
    assert(!m_entered);
    m_entered = true;

    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Switch based on state
    switch (m_state)
    {
        case normal:
            doNormalStuff();
            break;

        case flushing:
            doFlushingStuff();
            break;

        default:
            throw;
    }

    m_entered = false;
}

// Returns true if more pending tasks are needed
bool MPIMaster::needMorePendingTasks() const
{
    return m_pending_tasks.size() < m_comm_size;
}

// Do normal stuff
void MPIMaster::doNormalStuff()
{
    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Check for program termination interrupt
    if (programTerminated())
    {
        // Terminate Master
        m_state = terminated;
        return;
    }
    // Listen to Managers
    listenToManagers();

    // Pop finished tasks from busy queue and insert into finished queue
    popBusyQueue();

    // Call controller
    if (auto p_controller = m_p_controller.lock())
        p_controller->iterate();

    // Check for termination of Master and Managers
    if (m_master_manager_terminated)
    {
        // Send TERMINATE_MANAGER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_MANAGER_SIGNAL);

        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for flushing of Workers
    if (m_worker_flushed)
    {
        spdlog::debug("MPIMaster::doNormalStuff: Flushing workers!");

        // Send FLUSH_WORKER_SIGNAL to all Managers
        sendSignalToAllManagers(FLUSH_WORKER_SIGNAL);

        // Reset flag
        m_worker_flushed = false;

        // Switch to flushing state
        m_state = flushing;
        return;
    }

    // Delegate tasks to Managers
    delegateToManagers();
}

// Do flushing stuff
void MPIMaster::doFlushingStuff()
{
    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Check for program termination interrupt
    if (programTerminated())
    {
        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for termination of Master and Managers
    if (m_master_manager_terminated)
    {
        // Send TERMINATE_MANAGER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_MANAGER_SIGNAL);

        // Terminate Master
        m_state = terminated;
        return;
    }

    // Discard messages, error codes and signals
    discardMessagesErrorCodesAndSignals();

    // If all Managers are idle, transition to normal state
    if (m_idle_managers.size() == m_comm_size)
    {
        // Debug info
        if (spdlog::get(g_program_name)->level() <= spdlog::level::debug)
        {
            spdlog::debug("MPIMaster::doFlushingStuff: "
                    "transition to normal state!");
            spdlog::debug("Idle managers:");
            for (auto it = m_idle_managers.begin();
                    it != m_idle_managers.end(); it++)
                spdlog::debug("{}", *it);
            spdlog::debug("-- END --");
        }

        m_state = normal;
        return;
    }
}

// Push pending task
void MPIMaster::pushPendingTask(const std::string& input_string)
{
    m_pending_tasks.push(input_string);
}

// Returns whether finished tasks queue is empty
bool MPIMaster::finishedTasksEmpty() const
{
    return m_finished_tasks.empty();
}

// Returns reference to front finished task
TaskHandler& MPIMaster::frontFinishedTask()
{
    return m_finished_tasks.front();
}

// Pop finished task
void MPIMaster::popFinishedTask()
{
    m_finished_tasks.pop();
}

// Flush finished, busy and pending tasks
void MPIMaster::flush()
{
    m_worker_flushed = true;

    // Flush all TaskHandler queues
    flushQueues();
}

// Terminate Master
void MPIMaster::terminate()
{
    m_master_manager_terminated = true;
}

// Listen to messages from Managers.
void MPIMaster::listenToManagers()
{
    // While there are any incoming messages
    while (probeMessage())
    {
        // Probe manager
        int manager_rank = probeMessageManager();

        // Receive message
        std::string&& output_string = receiveMessage(manager_rank);

        // Receive error code
        int error_code = receiveErrorCode(manager_rank);

        // Record output string
        m_map_manager_to_task[manager_rank]->recordOutputAndErrorCode(
                output_string, error_code);

        // Mark manager as idle
        m_idle_managers.insert(manager_rank);
    }
}

// Pop finished tasks from busy queue and insert into finished queue
void MPIMaster::popBusyQueue()
{
    // If busy queue is empty, return immediately
    if (m_busy_tasks.empty())
        return;

    // While there are finished tasks (or tasks where errors occured) in the
    // front of the queue
    while (!m_busy_tasks.empty() && !m_busy_tasks.front().isPending())
    {
        spdlog::debug("MPIMaster::popBusyQueue: "
                "Moving TaskHandler from busy to finished!");
        spdlog::debug("finished, busy, pending: {}, {}, {}",
                m_finished_tasks.size(), m_busy_tasks.size(),
                m_pending_tasks.size());

        // Move TaskHandler to finished tasks
        m_finished_tasks.push(std::move(m_busy_tasks.front()));

        // Pop front TaskHandler from busy queue
        m_busy_tasks.pop();

        spdlog::debug("MPIMaster::popBusyQueue: "
                "Done moving TaskHandler from busy to finished!");
        spdlog::debug("finished, busy, pending: {}, {}, {}",
                m_finished_tasks.size(), m_busy_tasks.size(),
                m_pending_tasks.size());
    }
}

// Delegate to Managers
void MPIMaster::delegateToManagers()
{
    if (spdlog::get(g_program_name)->level() <= spdlog::level::debug)
    {
        spdlog::debug("MPIMaster::delegateToManagers: entered!");
        spdlog::debug("Idle managers:");
        for (auto it = m_idle_managers.begin();
                it != m_idle_managers.end(); it++)
            spdlog::debug("{}", *it);
        spdlog::debug("-- END --");
    }

    // While there are idle managers
    auto it = m_idle_managers.begin();
    for (; (it != m_idle_managers.end()) && !m_pending_tasks.empty(); it++)
    {
        spdlog::debug("MPIMaster::delegateToManagers: "
                "Moving TaskHandler from pending to busy!");
        spdlog::debug("finished, busy, pending: {}, {}, {}",
                m_finished_tasks.size(), m_busy_tasks.size(),
                m_pending_tasks.size());

        // Send message to Manager
        sendMessageToManager(*it, m_pending_tasks.front().getInputString());

        // Move pending TaskHandler to busy queue
        m_busy_tasks.push(std::move(m_pending_tasks.front()));

        // Pop front TaskHandler from pending queue
        m_pending_tasks.pop();

        // Set map from Manager to TaskHandler
        m_map_manager_to_task[*it] = &m_busy_tasks.back();

        spdlog::debug("MPIMaster::delegateToManagers: "
                "Done moving TaskHandler from pending to busy!");
        spdlog::debug("finished, busy, pending: {}, {}, {}",
                m_finished_tasks.size(), m_busy_tasks.size(),
                m_pending_tasks.size());
    }

    // Mark Managers as busy
    m_idle_managers.erase(m_idle_managers.begin(), it);

    if (spdlog::get(g_program_name)->level() <= spdlog::level::debug)
    {
        spdlog::debug("MPIMaster::delegateToManagers: exiting");
        spdlog::debug("Idle managers:");
        for (auto it = m_idle_managers.begin();
                it != m_idle_managers.end(); it++)
            spdlog::debug("{}", *it);
        spdlog::debug("-- END --");
    }
}

// Flush all task queues (finished, busy, pending)
void MPIMaster::flushQueues()
{
    while (!m_finished_tasks.empty()) m_finished_tasks.pop();
    while (!m_busy_tasks.empty()) m_busy_tasks.pop();
    while (!m_pending_tasks.empty()) m_pending_tasks.pop();
}

// Discard any messages and signals until all Managers are idle
void MPIMaster::discardMessagesErrorCodesAndSignals()
{
    // While there are any incoming messages
    while (probeMessage())
    {
        // Probe manager
        int manager_rank = probeMessageManager();

        // Receive and discard message
        receiveMessage(manager_rank);

        // Receive and discard error code
        receiveErrorCode(manager_rank);

        // Mark manager as idle
        m_idle_managers.insert(manager_rank);
    }

    // While there are any incoming signals
    while (probeSignal())
    {
        // Probe manager
        int manager_rank = probeSignalManager();

        // If it a cancellation signal, mark manager as idle
        if (receiveSignal(manager_rank) == WORKER_FLUSHED_SIGNAL)
            m_idle_managers.insert(manager_rank);
    }
}

// Probe for message
bool MPIMaster::probeMessage() const
{
    return iprobe_wrapper(MPI_ANY_SOURCE, MANAGER_MSG_TAG, MPI_COMM_WORLD);
}

// Probe for signal
bool MPIMaster::probeSignal() const
{
    return iprobe_wrapper(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG, MPI_COMM_WORLD);
}

// Probe for Manager rank of incoming message
int MPIMaster::probeMessageManager() const
{
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MANAGER_MSG_TAG, MPI_COMM_WORLD, &status);
    return static_cast<int>(status.MPI_SOURCE);
}

// Probe for Manager rank of incoming signal
int MPIMaster::probeSignalManager() const
{
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG, MPI_COMM_WORLD, &status);
    return static_cast<int>(status.MPI_SOURCE);
}

// Receive message from Manager
std::string MPIMaster::receiveMessage(int manager_rank) const
{
    // Sanity check: probeMessage must return true
    assert(probeMessage());

    return receive_string(MPI_COMM_WORLD, manager_rank, MANAGER_MSG_TAG);
}

// Receive signal from Manager
int MPIMaster::receiveSignal(int manager_rank) const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    return receive_integer(MPI_COMM_WORLD, manager_rank, MANAGER_SIGNAL_TAG);
}

// Receive error code from Manager
int MPIMaster::receiveErrorCode(int manager_rank) const
{
    return receive_integer(MPI_COMM_WORLD, manager_rank,
            MANAGER_ERROR_CODE_TAG);
}

// Send message to a Manager
void MPIMaster::sendMessageToManager(int manager_rank,
        const std::string& message_string)
{
    if (spdlog::get(g_program_name)->level() <= spdlog::level::debug)
    {
        spdlog::debug("MPIMaster::sendMessageToManager: "
                "sending to manager_rank {}",
                " and message:\n{}", manager_rank, message_string);
        spdlog::debug("Idle managers:");
        for (auto it = m_idle_managers.begin();
                it != m_idle_managers.end(); it++)
            spdlog::debug("{}", *it);
        spdlog::debug("-- END --");
    }

    // Ensure previous message has finished sending
    MPI_Wait(&m_message_requests[manager_rank], MPI_STATUS_IGNORE);

    // Store message string in buffer
    m_message_buffers[manager_rank].assign(message_string);

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    MPI_Isend(
            m_message_buffers[manager_rank].c_str(),
            m_message_buffers[manager_rank].size() + 1,
            MPI_CHAR, manager_rank, MASTER_MSG_TAG,
            MPI_COMM_WORLD,
            &m_message_requests[manager_rank]);
}

// Send signal to all Managers
void MPIMaster::sendSignalToAllManagers(int signal)
{
    // Ensure previous signals have finished sending
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        MPI_Wait(&m_signal_requests[manager_rank], MPI_STATUS_IGNORE);

    // Store signal in buffer
    m_signal_buffer = signal;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        MPI_Isend(&m_signal_buffer, 1, MPI_INT, manager_rank,
                MASTER_SIGNAL_TAG, MPI_COMM_WORLD,
                &m_signal_requests[manager_rank]);
}
