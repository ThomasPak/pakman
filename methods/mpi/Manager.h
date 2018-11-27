#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <cassert>

#include <mpi.h>

#include "ProcessHandler.h"
#include "common.h"
#include "../types.h"

class Manager
{
    public:

        // Enumerate type for Manager states
        enum state_t { idle, busy, terminated };

        // Construct from command and pointer to program terminated flag
        Manager(cmd_t command, bool *p_program_terminated,
                process_t process_type) :
            m_command(command),
            m_p_program_terminated(p_program_terminated),
            m_process_type(process_type)
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

        // Probe for input
        bool probeInput() const;

        // Receive input
        std::string receiveInput() const;

        // Probe for signal
        bool probeSignal() const;

        // Receive signal
        int receiveSignal() const;

        // Create process
        void createProcess(const std::string& input_string);

        // Terminate process
        void terminateProcess();

        // Send output to Master
        void sendOutputToMaster(const std::string& output_string) const;

        // Send signal to Master
        void sendSignalToMaster(int signal) const;

        /**** Member variables ****/
        // Command for process
        const std::string m_command;

        // Process type (forked process vs MPI process)
        const process_t m_process_type;

        // Initial state is idle
        state_t m_state = idle;

        // Pointer to process handler
        AbstractProcessHandler *m_p_process_handler = nullptr;

        // Pointer to program terminated flag
        bool *m_p_program_terminated;
};

#endif // MANAGER_H
