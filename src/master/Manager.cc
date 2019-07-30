#include <string>
#include <memory>

#include <assert.h>

#include <mpi.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "mpi/mpi_common.h"
#include "mpi/mpi_utils.h"

#include "ForkedWorkerHandler.h"
#include "MPIWorkerHandler.h"

#include "Manager.h"

// Construct from simulator, pointer to program terminated flag, and
// Worker type (forked vs MPI)
Manager::Manager(const Command &simulator, worker_t worker_type,
        bool *p_program_terminated) :
    m_simulator(simulator),
    m_worker_type(worker_type),
    m_p_program_terminated(p_program_terminated)
{
}

// Destroy MPI_Request objects
Manager::~Manager()
{
    // If MPI_Finalize has been called, nothing needs to be done
    int finalized = 0;
    MPI_Finalized(&finalized);

    if (finalized)
        return;

    // Else free any non-null requests
    if (m_message_request != MPI_REQUEST_NULL)
        MPI_Request_free(&m_message_request);
    if (m_signal_request != MPI_REQUEST_NULL)
        MPI_Request_free(&m_signal_request);
    if (m_error_code_request != MPI_REQUEST_NULL)
        MPI_Request_free(&m_error_code_request);
}

// Probe whether Manager is active
bool Manager::isActive() const
{
    return m_state != terminated;
}

// Iterate
void Manager::iterate()
{
    // This function should never be called if the Manager has
    // terminated
    assert(m_state != terminated);

    // Switch based on state
    switch (m_state)
    {
        case idle:
            doIdleStuff();
            break;

        case busy:
            doBusyStuff();
            break;

        default:
            throw;
    }
}

// Do idle stuff
void Manager::doIdleStuff()
{
    // Sanity check: m_p_worker_handler should be the null pointer
    assert(!m_p_worker_handler);

    // Check for program termination interrupt
    if (*m_p_program_terminated)
    {
        // Terminate Manager
        m_state = terminated;
        return;
    }

    // Check for Manager termination signal
    if (probeSignal())
    {
        switch (receiveSignal())
        {
            case TERMINATE_MANAGER_SIGNAL:

                spdlog::debug("Idle manager {}/{}: received "
                        "TERMINATE_MANAGER_SIGNAL!",
                        get_mpi_comm_world_rank(), get_mpi_comm_world_size());

                // Terminate Manager
                m_state = terminated;
                return;

            // An idle Manager can receive FLUSH_WORKER_SIGNAL when the Master
            // has not yet checked for the Manager's message.  In this case,
            // nothing needs to be done
            case FLUSH_WORKER_SIGNAL:

                spdlog::debug("Idle manager {}/{}: received "
                        "FLUSH_WORKER_SIGNAL!",
                        get_mpi_comm_world_rank(), get_mpi_comm_world_size());

                return;

            // TERMINATE_MANAGER_SIGNAL and FLUSH_WORKER_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check for message
    if (probeMessage())
    {
        spdlog::debug("Idle manager {}/{}: received message!",
                get_mpi_comm_world_rank(), get_mpi_comm_world_size());

        // Receive input string and create new Worker
        std::string input_string = receiveMessage();
        createWorker(input_string);

        // Switch to busy state
        m_state = busy;
        return;
    }
}

// Do busy stuff
void Manager::doBusyStuff()
{
    // Sanity check: m_p_worker_handler should not be the null pointer
    assert(m_p_worker_handler);

    // Check for program termination interrupt
    if (*m_p_program_terminated)
    {
        // Terminate Worker
        terminateWorker();

        // Terminate Manager
        m_state = terminated;
        return;
    }

    // Check for Manager or Worker termination signal
    if (probeSignal())
    {
        switch (receiveSignal())
        {
            case TERMINATE_MANAGER_SIGNAL:

                spdlog::debug("Busy manager {}/{}: received "
                        "TERMINATE_MANAGER_SIGNAL!",
                        get_mpi_comm_world_rank(), get_mpi_comm_world_size());

                // Terminate Worker
                terminateWorker();

                // Terminate Manager
                m_state = terminated;
                return;

            case FLUSH_WORKER_SIGNAL:

                spdlog::debug("Busy manager {}/{}: received "
                        "FLUSH_WORKER_SIGNAL!",
                        get_mpi_comm_world_rank(), get_mpi_comm_world_size());

                // Flush Worker
                flushWorker();

                // Send WORKER_FLUSHED_SIGNAL result
                sendSignalToMaster(WORKER_FLUSHED_SIGNAL);

                // Switch to idle state
                m_state = idle;
                return;

            // TERMINATE_MANAGER_SIGNAL and FLUSH_WORKER_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check if Worker has finished
    if (m_p_worker_handler->isDone())
    {
        spdlog::debug("Busy manager {}/{}: Worker is done!",
                get_mpi_comm_world_rank(), get_mpi_comm_world_size());

        // Get output string
        std::string output_string = m_p_worker_handler->getOutput();

        // Get error code
        int error_code = m_p_worker_handler->getErrorCode();

        // Send output string to master
        sendMessageToMaster(output_string);

        // Send error code to master
        sendErrorCodeToMaster(error_code);

        // Flush Worker
        flushWorker();

        // Switch to idle state
        m_state = idle;
        return;
    }

    // Sanity check: No message should be received in the busy state
    assert(probeMessage() == false);
}

// Create Worker
void Manager::createWorker(const std::string& input_string)
{
    // Sanity check: m_p_worker_handler should be the null pointer
    assert(!m_p_worker_handler);

    // Switch on Worker type
    switch (m_worker_type)
    {
        // Fork Worker
        case forked_worker:
            m_p_worker_handler =
                std::unique_ptr<ForkedWorkerHandler>(
                        new ForkedWorkerHandler(m_simulator, input_string));
            break;

        // Spawn MPI Worker
        case mpi_worker:
            m_p_worker_handler =
                std::unique_ptr<MPIWorkerHandler>(
                        new MPIWorkerHandler(m_simulator, input_string));
            break;

        default:
            throw std::runtime_error("Worker type not recognised");
    }
}

// Flush Worker
void Manager::flushWorker()
{
    // Sanity check: This function should not be called when
    // m_p_worker_handler is the null pointer
    assert(m_p_worker_handler);

    // Reset m_p_worker_handler to null pointer,
    // this flushes the worker
    m_p_worker_handler.reset();
}

// Terminate Worker
void Manager::terminateWorker()
{
    // Sanity check: This function should not be called when
    // m_p_worker_handler is the null pointer
    assert(m_p_worker_handler);

    // Reset m_p_worker_handler to null pointer
    m_p_worker_handler.reset();
}

// Probe for message
bool Manager::probeMessage() const
{
    return iprobe_wrapper(MASTER_RANK, MASTER_MSG_TAG, MPI_COMM_WORLD);
}

// Probe for signal
bool Manager::probeSignal() const
{
    return iprobe_wrapper(MASTER_RANK, MASTER_SIGNAL_TAG, MPI_COMM_WORLD);
}

// Receive message
std::string Manager::receiveMessage() const
{
    // Sanity check: probeMessage must return true
    assert(probeMessage());

    return receive_string(MPI_COMM_WORLD, MASTER_RANK, MASTER_MSG_TAG);
}

// Receive signal
int Manager::receiveSignal() const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    return receive_integer(MPI_COMM_WORLD, MASTER_RANK, MASTER_SIGNAL_TAG);
}

// Send message to Master
void Manager::sendMessageToMaster(const std::string& message_string)
{
    // Ensure previous message has finished sending
    MPI_Wait(&m_message_request, MPI_STATUS_IGNORE);

    // Store message string in buffer
    m_message_buffer.assign(message_string);

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    MPI_Isend(
            m_message_buffer.c_str(),
            m_message_buffer.size() + 1,
            MPI_CHAR, MASTER_RANK, MANAGER_MSG_TAG,
            MPI_COMM_WORLD,
            &m_message_request);
}

// Send signal to Master
void Manager::sendSignalToMaster(int signal)
{
    // Ensure previous signal has finished sending
    MPI_Wait(&m_signal_request, MPI_STATUS_IGNORE);

    // Store signal in buffer
    m_signal_buffer = signal;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    MPI_Isend(&m_signal_buffer, 1, MPI_INT, MASTER_RANK,
            MANAGER_SIGNAL_TAG, MPI_COMM_WORLD,
            &m_signal_request);
}

// Send error code to Master
void Manager::sendErrorCodeToMaster(int error_code)
{
    // Ensure previous error code has finished sending
    MPI_Wait(&m_error_code_request, MPI_STATUS_IGNORE);

    // Store error_code in buffer
    m_error_code_buffer = error_code;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    MPI_Isend(&m_error_code_buffer, 1, MPI_INT, MASTER_RANK,
            MANAGER_ERROR_CODE_TAG, MPI_COMM_WORLD,
            &m_error_code_request);
}
