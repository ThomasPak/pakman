#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <cassert>

#include <mpi.h>

#include "ProcessHandler.h"
#include "../types.h"

class Manager
{
    public:

        // Enumerate type for Manager states
        enum state_t { idle, busy, terminated };

        // Construct from command, pointer to program terminated flag, and
        // process type (forked vs MPI)
        Manager(cmd_t command, process_t process_type,
                bool *p_program_terminated) :
            m_command(command),
            m_process_type(process_type),
            m_p_program_terminated(p_program_terminated)
        {
        }

        // Destructor
        ~Manager()
        {
            if (m_p_process_handler)
                delete m_p_process_handler;
        }

        // Get state of Manager
        state_t getState() const
        {
            return m_state;
        }

        // Probe whether Manager is active
        bool isActive() const
        {
            return m_state != terminated;
        }

        // Iterate
        void iterate()
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

    private:

        /**** Member functions ****/
        // Do idle stuff
        void doIdleStuff();

        // Do busy stuff
        void doBusyStuff();

        // Create process
        void createProcess(const std::string& input_string);

        // Terminate process
        void terminateProcess();

        // Probe for message
        bool probeMessage() const;

        // Probe for signal
        bool probeSignal() const;

        // Receive message
        std::string receiveMessage() const;

        // Receive signal
        int receiveSignal() const;

        // Send message to Master
        void sendMessageToMaster(const std::string& message_string);

        // Send signal to Master
        void sendSignalToMaster(int signal);

        /**** Member variables ****/
        // Initial state is idle
        state_t m_state = idle;

        // Command for process
        const std::string m_command;

        // Process type (forked process vs MPI process)
        const process_t m_process_type;

        // Pointer to program terminated flag
        bool *m_p_program_terminated;

        // Pointer to process handler
        AbstractProcessHandler *m_p_process_handler = nullptr;

        // Message buffer
        std::string m_message_buffer;

        // Message request
        MPI::Request m_message_request;

        // Signal buffer
        int m_signal_buffer;

        // Signal request
        MPI::Request m_signal_request;
};

#endif // MANAGER_H
