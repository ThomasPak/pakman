#include <memory>
#include <string>
#include <cassert>

#include <mpi.h>

#include "../common.h"
#include "mpi_common.h"
#include "WorkerHandler.h"
#include "Manager.h"

#ifndef NDEBUG
#include <iostream>
#endif

// Construct from command, pointer to program terminated flag, and
// Worker type (forked vs MPI)
Manager::Manager(const cmd_t &command, worker_t worker_type,
        bool *p_program_terminated) :
    m_command(command),
    m_worker_type(worker_type),
    m_p_program_terminated(p_program_terminated)
{
}

// Get state of Manager
Manager::state_t Manager::getState() const
{
    return m_state;
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
                // Terminate Manager
                m_state = terminated;
                return;

            // An idle Manager can receive TERMINATE_WORKER_SIGNAL when the
            // Master has not yet checked for the Manager's message.
            // In this case, nothing needs to be done
            case TERMINATE_WORKER_SIGNAL:
                return;

            // TERMINATE_MANAGER_SIGNAL and TERMINATE_WORKER_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check for message
    if (probeMessage())
    {
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
                // Terminate Worker
                terminateWorker();

                // Terminate Manager
                m_state = terminated;
                return;

            case TERMINATE_WORKER_SIGNAL:
#ifndef NDEBUG
                std::cerr << "Manager: sending WORKER_CANCELLED_SIGNAL to Master\n";
#endif
                // Terminate Worker
                terminateWorker();

                // Send WORKER_CANCELLED_SIGNAL result
                sendSignalToMaster(WORKER_CANCELLED_SIGNAL);

                // Switch to idle state
                m_state = idle;
                return;

            // TERMINATE_MANAGER_SIGNAL and TERMINATE_WORKER_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check if Worker has finished
    if (m_p_worker_handler->isDone())
    {
        // Get output string
        std::string output_string = m_p_worker_handler->getOutput();

        // Send output string to master
        sendMessageToMaster(output_string);

        // Terminate Worker handler
        terminateWorker();

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
                        new ForkedWorkerHandler(m_command, input_string));
            break;

        // Spawn MPI Worker
        case mpi_worker:
            m_p_worker_handler =
                std::unique_ptr<MPIWorkerHandler>(
                        new MPIWorkerHandler(m_command, input_string));
            break;

        default:
            throw;
    }
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
    return MPI::COMM_WORLD.Iprobe(MASTER_RANK, INPUT_TAG);
}

// Probe for signal
bool Manager::probeSignal() const
{
    return MPI::COMM_WORLD.Iprobe(MASTER_RANK, MASTER_SIGNAL_TAG);
}

// Receive message
std::string Manager::receiveMessage() const
{
    // Sanity check: probeMessage must return true
    assert(probeMessage());

    // Probe message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MASTER_RANK, INPUT_TAG, status);

    // Sanity check on message
    assert(status.Get_tag() == INPUT_TAG);
    assert(status.Get_source() == MASTER_RANK);

    // Receive message from Master
    int count = status.Get_count(MPI::CHAR);
    char *buffer = new char[count];
    MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, MASTER_RANK, INPUT_TAG);

    // Return message as string
    std::string message(buffer);
    delete[] buffer;
    return message;
}

// Receive signal
int Manager::receiveSignal() const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    // Probe signal message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MASTER_RANK, MASTER_SIGNAL_TAG, status);

    // Sanity check on signal, which has to be a single integer
    assert(status.Get_tag() == MASTER_SIGNAL_TAG);
    assert(status.Get_source() == MASTER_RANK);
    assert(status.Get_count(MPI::INT) == 1);

    // Receive signal from Master
    int signal = 0;
    MPI::COMM_WORLD.Recv(&signal, 1, MPI::INT, MASTER_RANK, MASTER_SIGNAL_TAG);

    // Return signal as integer
    return signal;
}

// Send message to Master
void Manager::sendMessageToMaster(const std::string& message_string)
{
    // Ensure previous message has finished sending
    m_message_request.Wait();

    // Store message string in buffer
    m_message_buffer.assign(message_string);

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    m_message_request = MPI::COMM_WORLD.Isend(m_message_buffer.c_str(),
            m_message_buffer.size() + 1, MPI::CHAR, MASTER_RANK, OUTPUT_TAG);
}

// Send signal to Master
void Manager::sendSignalToMaster(int signal)
{
    // Ensure previous signal has finished sending
    m_signal_request.Wait();

    // Store signal in buffer
    m_signal_buffer = signal;

    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    m_signal_request = MPI::COMM_WORLD.Isend(&m_signal_buffer, 1, MPI::INT,
            MASTER_RANK, MANAGER_SIGNAL_TAG);
}
