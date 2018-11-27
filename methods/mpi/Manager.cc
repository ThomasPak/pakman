#include <string>
#include <cassert>

#include <mpi.h>
#include <signal.h>

// TODO: remove run_simulation.h when output string is sent to Master as a
// whole
#include "../run_simulation.h"
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

            // TERMINATE_MANAGER_SIGNAL is the only valid signal in the idle
            // Manager state
            default:
#ifndef NDEBUG
                std::cerr << "Manager: I received a signal that's not "
                    "TERMINATE_MANAGER_SIGNAL!!\n";
#endif
                throw;
        }
    }

    // Check for input message
    if (probeInput())
    {
        // Receive input string and create new process
        std::string input_string = receiveInput();
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
                // Terminate process
                terminateProcess();

                // Send CANCEL result
                sendOutputToMaster(CANCEL);

                // Switch to idle state
                m_state = idle;
                return;

            // TERMINATE_MANAGER_SIGNAL and TERMINATE_PROCESS_SIGNAL are the
            // only valid signals in the busy Manager state
            default:
                throw;
        }
    }

    // Check if process has finished
    if (m_p_process_handler->isDone())
    {
        // Get output string
        std::string result_str = m_p_process_handler->getOutput();

        // TODO: send entire output string to master
        // Send result to master
        int result = simulation_result(result_str);
        sendOutputToMaster(result);

        // Terminate process handler
        terminateProcess();

        // Switch to idle state
        m_state = idle;
        return;
    }

    // Sanity check: No input message should be received in the busy state
    assert(probeInput() == false);
}

// Probe for input
bool Manager::probeInput() const
{
    return MPI::COMM_WORLD.Iprobe(MASTER, PARAMETER_TAG);
}

// Receive input
std::string Manager::receiveInput() const
{
    // Sanity check: probeInput must return true
    assert(probeInput());

    // Probe input message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MASTER, PARAMETER_TAG, status);

    // Sanity check on input
    assert(status.Get_tag() == PARAMETER_TAG);
    assert(status.Get_source() == MASTER);

    // Receive input from Master
    int count = status.Get_count(MPI::CHAR);
    char *buffer = new char[count];
    MPI::COMM_WORLD.Recv(buffer, count, MPI::CHAR, MASTER, PARAMETER_TAG);

    // Return input as string
    std::string input_string(buffer);
    delete[] buffer;
    return input_string;
}

// Probe for signal
bool Manager::probeSignal() const
{
    return MPI::COMM_WORLD.Iprobe(MASTER, SIGNAL_TAG);
}

// Receive signal
int Manager::receiveSignal() const
{
    // Sanity check: probeSignal must return true
    assert(probeSignal());

    // Probe signal message to get status
    MPI::Status status;
    MPI::COMM_WORLD.Probe(MASTER, SIGNAL_TAG, status);

    // Sanity check on signal, which has to be a single integer
    assert(status.Get_tag() == SIGNAL_TAG);
    assert(status.Get_source() == MASTER);
    assert(status.Get_count(MPI::INT) == 1);

    // Receive signal from Master
    int signal = 0;
    MPI::COMM_WORLD.Recv(&signal, 1, MPI::INT, MASTER, SIGNAL_TAG);

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

// TODO: send output string instead of result
// Send result to Master
void Manager::sendOutputToMaster(int result) const
{
    // Note: Isend is used here to avoid deadlock since the Master and the root
    // Manager are executed by the same process
    MPI::COMM_WORLD.Isend(&result, 1, MPI::INT, MASTER, RESULT_TAG);
}
