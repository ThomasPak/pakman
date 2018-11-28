#include <string>
#include <cassert>

#include <mpi.h>

#include "common.h"
#include "ProcessHandler.h"
#include "Manager.h"

#ifndef NDEBUG
#include <iostream>
#endif

// Do idle stuff
void Manager::doIdleStuff()
{
    // Sanity check: m_p_process_handler should be the null pointer
    assert(m_p_process_handler == nullptr);

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

            // An idle Manager can receive TERMINATE_PROCESS_SIGNAL when the
            // Master has not yet checked for the Manager's message.
            // In this case, nothing needs to be done
            case TERMINATE_PROCESS_SIGNAL:
                return;

            // TERMINATE_MANAGER_SIGNAL and TERMINATE_PROCESS_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check for message
    if (probeMessage())
    {
        // Receive input string and create new process
        std::string input_string = receiveMessage();
        createProcess(input_string);

        // Switch to busy state
        m_state = busy;
        return;
    }
}

// Do busy stuff
void Manager::doBusyStuff()
{
    // Sanity check: m_p_process_handler should not be the null pointer
    assert(m_p_process_handler != nullptr);

    // Check for program termination interrupt
    if (*m_p_program_terminated)
    {
        // Terminate process
        terminateProcess();

        // Terminate Manager
        m_state = terminated;
        return;
    }

    // Check for Manager or process termination signal
    if (probeSignal())
    {
        switch (receiveSignal())
        {
            case TERMINATE_MANAGER_SIGNAL:
                // Terminate process
                terminateProcess();

                // Terminate Manager
                m_state = terminated;
                return;

            case TERMINATE_PROCESS_SIGNAL:
#ifndef NDEBUG
                std::cerr << "Manager: sending PROCESS_CANCELLED_SIGNAL to Master\n";
#endif
                // Terminate process
                terminateProcess();

                // Send PROCESS_CANCELLED_SIGNAL result
                sendSignalToMaster(PROCESS_CANCELLED_SIGNAL);

                // Switch to idle state
                m_state = idle;
                return;

            // TERMINATE_MANAGER_SIGNAL and TERMINATE_PROCESS_SIGNAL are the
            // only valid Master signals
            default:
                throw;
        }
    }

    // Check if process has finished
    if (m_p_process_handler->isDone())
    {
        // Get output string
        std::string output_string = m_p_process_handler->getOutput();

        // Send output string to master
        sendMessageToMaster(output_string);

        // Terminate process handler
        terminateProcess();

        // Switch to idle state
        m_state = idle;
        return;
    }

    // Sanity check: No message should be received in the busy state
    assert(probeMessage() == false);
}

// Probe for message
bool Manager::probeMessage() const
{
    return MPI::COMM_WORLD.Iprobe(MASTER_RANK, INPUT_TAG);
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

// Probe for signal
bool Manager::probeSignal() const
{
    return MPI::COMM_WORLD.Iprobe(MASTER_RANK, MASTER_SIGNAL_TAG);
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

// Create process
void Manager::createProcess(const std::string& input_string)
{
    // Sanity check: m_p_process_handler should be the null pointer
    assert(m_p_process_handler == nullptr);

    // Switch on process type
    switch (m_process_type)
    {
        // Fork process
        case forked_process:
            m_p_process_handler =
                new ForkedProcessHandler(m_command, input_string);
            break;

        // Spawn MPI process
        case mpi_process:
            m_p_process_handler =
                new MPIProcessHandler(m_command, input_string);
            break;

        default:
            throw;
    }
}

// Terminate process
void Manager::terminateProcess()
{
    // Sanity check: This function should not be called when
    // m_p_process_handler is the null pointer
    assert(m_p_process_handler != nullptr);

    // Delete process handler and reset m_p_process_handler to null pointer
    delete m_p_process_handler;
    m_p_process_handler = nullptr;
}

// Send message to Master
void Manager::sendMessageToMaster(const std::string& message_string)
{
    // Ensure previous message message has finished sending
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
    m_signal_request = MPI::COMM_WORLD.Isend(&m_signal_buffer,  1, MPI::INT,
            MASTER_RANK, MANAGER_SIGNAL_TAG);
}
