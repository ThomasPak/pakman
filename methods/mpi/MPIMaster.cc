#include <memory>
#include <string>
#include <queue>
#include <cassert>
#include <mpi.h>

#include "mpi_common.h"

#include "MPIMaster.h"

#ifndef NDEBUG
#include <iostream>
#endif

// Construct from pointer to program terminated flag
MPIMaster::MPIMaster(bool *p_program_terminated) :
    AbstractMaster(p_program_terminated),
    m_comm_size(MPI::COMM_WORLD.Get_size()),
    m_map_manager_to_task(MPI::COMM_WORLD.Get_size()),
    m_message_buffers(MPI::COMM_WORLD.Get_size()),
    m_message_requests(MPI::COMM_WORLD.Get_size()),
    m_signal_requests(MPI::COMM_WORLD.Get_size())
{
    for (int i = 0; i < m_comm_size; i++)
        m_idle_managers.insert(i);
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
    static bool entered = false;
    if (entered) throw;
    entered = true;

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

    entered = false;
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

    // Check for termination of Master and Managers
    if (m_master_manager_terminated)
    {
        // Send TERMINATE_MANAGER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_MANAGER_SIGNAL);

        // Terminate Master
        m_state = terminated;
        return;
    }

    // Check for termination of Workers
    if (m_worker_terminated)
    {
        // Send TERMINATE_WORKER_SIGNAL to all Managers
        sendSignalToAllManagers(TERMINATE_WORKER_SIGNAL);

        // Flush all TaskHandler queues
        flushQueues();

        // Reset flag
        m_worker_terminated = false;

        // Switch to flushing state
        m_state = flushing;
        return;
    }

    // Listen to Managers
    listenToManagers();

    // Pop finished tasks from busy queue and insert into finished queue
    popBusyQueue();

    // Call controller
    if (std::shared_ptr<AbstractController> p_controller =
            m_p_controller.lock())
        p_controller->iterate();

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

    // Discard message and signals
    discardMessagesAndSignals();

    // If all Managers are idle, transition to normal state
    if (m_idle_managers.size() == m_comm_size)
    {
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
AbstractMaster::TaskHandler& MPIMaster::frontFinishedTask()
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
    m_worker_terminated = true;
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
        m_map_manager_to_task[manager_rank]->recordOutputAndErrorCode(output_string,
                error_code);

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
#ifndef NDEBUG
        std::cerr << "DEBUG: MPIMaster::popBusyQueue: ";
        std::cerr << "Moving TaskHandler from busy to finished!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
        // Move TaskHandler to finished tasks
        m_finished_tasks.push(std::move(m_busy_tasks.front()));

        // Pop front TaskHandler from busy queue
        m_busy_tasks.pop();
#ifndef NDEBUG
        std::cerr << "DEBUG: MPIMaster::popBusyQueue: ";
        std::cerr << "Done moving TaskHandler from busy to finished!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
    }
}

// Delegate to Managers
void MPIMaster::delegateToManagers()
{
    // While there are idle managers
    auto it = m_idle_managers.begin();
    for (; (it != m_idle_managers.end()) && !m_pending_tasks.empty(); it++)
    {
#ifndef NDEBUG
        std::cerr << "DEBUG: MPIMaster::delegateToManagers: ";
        std::cerr << "Moving TaskHandler from pending to busy!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
        // Send message to Manager
        sendMessageToManager(*it, m_pending_tasks.front().getInputString());

        // Move pending TaskHandler to busy queue
        m_busy_tasks.push(std::move(m_pending_tasks.front()));

        // Pop front TaskHandler from pending queue
        m_pending_tasks.pop();

        // Set map from Manager to TaskHandler
        m_map_manager_to_task[*it] = &m_busy_tasks.back();
#ifndef NDEBUG
        std::cerr << "DEBUG: MPIMaster::delegateToManagers: ";
        std::cerr << "Done moving TaskHandler from pending to busy!\n";
        std::cerr << "DEBUG: finished, busy, pending: "
            << m_finished_tasks.size() << ", " << m_busy_tasks.size()
            << ", " << m_pending_tasks.size() << std::endl;
#endif
    }

    // Mark Managers as busy
    m_idle_managers.erase(m_idle_managers.begin(), it);
}

// Flush all task queues (finished, busy, pending)
void MPIMaster::flushQueues()
{
    while (!m_finished_tasks.empty()) m_finished_tasks.pop();
    while (!m_busy_tasks.empty()) m_busy_tasks.pop();
    while (!m_pending_tasks.empty()) m_pending_tasks.pop();
}

// Discard any messages and signals until all Managers are idle
void MPIMaster::discardMessagesAndSignals()
{
    // While there are any incoming messages
    while (probeMessage())
    {
        // Probe manager
        int manager_rank = probeMessageManager();

        // Receive and discard message
        receiveMessage(manager_rank);

        // Mark manager as idle
        m_idle_managers.insert(manager_rank);
    }

    // While there are any incoming signals
    while (probeSignal())
    {
        // Probe manager
        int manager_rank = probeSignalManager();

        // If it a cancellation signal, mark manager as idle
        if (receiveSignal(manager_rank) == WORKER_CANCELLED_SIGNAL)
            m_idle_managers.insert(manager_rank);
    }
}

// Probe for message
bool MPIMaster::probeMessage() const
{
    return MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, MANAGER_MSG_TAG);
}

// Probe for signal
bool MPIMaster::probeSignal() const
{
    return MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG);
}

// Probe for error code
bool MPIMaster::probeErrorCode() const
{
    return MPI::COMM_WORLD.Iprobe(MPI_ANY_SOURCE, MANAGER_ERROR_CODE_TAG);
}
// Probe for Manager rank of incoming message
int MPIMaster::probeMessageManager() const
{
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, MANAGER_MSG_TAG, status);
    return status.Get_source();
}

// Probe for Manager rank of incoming signal
int MPIMaster::probeSignalManager() const
{
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MPI_ANY_SOURCE, MANAGER_SIGNAL_TAG, status);
    return status.Get_source();
}

// Receive message from Manager
std::string MPIMaster::receiveMessage(int manager_rank) const
{
    // Sanity check: probeMessage must return true
    assert(probeMessage());

    // Probe message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(manager_rank, MANAGER_MSG_TAG, status);

    // Sanity check on message
    assert(status.Get_tag() == MANAGER_MSG_TAG);
    assert(status.Get_source() == manager_rank);

    // Receive message from Manager
    int count = status.Get_count(MPI::CHAR);
    char *buffer = new char[count];
    MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, manager_rank, MANAGER_MSG_TAG);

    // Return input as string
    std::string input_string(buffer);
    delete[] buffer;
    return input_string;
}

// Receive signal from Manager
int MPIMaster::receiveSignal(int manager_rank) const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    // Probe signal message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(manager_rank, MANAGER_SIGNAL_TAG, status);

    // Sanity check on signal, which has to be a single integer
    assert(status.Get_tag() == MANAGER_SIGNAL_TAG);
    assert(status.Get_source() == manager_rank);
    assert(status.Get_count(MPI::INT) == 1);

    // Receive signal from Manager
    int signal;
    MPI::COMM_WORLD.Recv(&signal, 1, MPI::INT, manager_rank,
            MANAGER_SIGNAL_TAG);

    // Return signal as integer
    return signal;
}

// Receive error code from Manager
int MPIMaster::receiveErrorCode(int manager_rank) const
{
    // Sanity check: probeErrorCode must return true
    assert(probeErrorCode());

    // Probe signal message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(manager_rank, MANAGER_ERROR_CODE_TAG, status);

    // Sanity check on signal, which has to be a single integer
    assert(status.Get_tag() == MANAGER_ERROR_CODE_TAG);
    assert(status.Get_source() == manager_rank);
    assert(status.Get_count(MPI::INT) == 1);

    // Receive error code from Manager
    int error_code;
    MPI::COMM_WORLD.Recv(&error_code, 1, MPI::INT, manager_rank,
            MANAGER_ERROR_CODE_TAG);

    // Return signal as integer
    return error_code;
}

// Send message to a Manager
void MPIMaster::sendMessageToManager(int manager_rank,
        const std::string& message_string)
{
#ifndef NDEBUG
    std::cerr << "MPIMaster::sendMessageToManager: sending to manager_rank " << manager_rank
        << " and message:\n" << message_string << std::endl;
    std::cerr << "Idle managers:\n";
    for (auto it = m_idle_managers.begin(); it != m_idle_managers.end(); it++)
        std::cerr << *it << std::endl;
    std::cerr << "-- END --\n";
#endif
    // Ensure previous message has finished sending
    m_message_requests[manager_rank].Wait();

    // Store message string in buffer
    m_message_buffers[manager_rank].assign(message_string);

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    m_message_requests[manager_rank] = MPI::COMM_WORLD.Isend(
            m_message_buffers[manager_rank].c_str(),
            m_message_buffers[manager_rank].size() + 1,
            MPI::CHAR, manager_rank, MASTER_MSG_TAG);
}

// Send signal to all Managers
void MPIMaster::sendSignalToAllManagers(int signal)
{
    // Ensure previous signals have finished sending
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        m_signal_requests[manager_rank].Wait();

    // Store signal in buffer
    m_signal_buffer = signal;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    for (int manager_rank = 0; manager_rank < m_comm_size; manager_rank++)
        m_signal_requests[manager_rank] =
            MPI::COMM_WORLD.Isend(&m_signal_buffer, 1, MPI::INT, manager_rank,
                    MASTER_SIGNAL_TAG);
}
