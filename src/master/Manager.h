#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <memory>

#include <assert.h>

#include <mpi.h>

#include "AbstractWorkerHandler.h"

class Manager
{
    public:

        // Enumerate type for Worker types
        enum worker_t
        {
            forked_worker,
            mpi_worker,
            persistent_mpi_worker
        };

        // Enumerate type for Manager states
        enum state_t { idle, busy, terminated };

        // Construct from command, pointer to program terminated flag, and
        // Worker type (forked vs MPI)
        Manager(const Command &command, worker_t worker_type,
                bool *p_program_terminated);

        // Destructor
        ~Manager();

        // Get state of Manager
        state_t getState() const;

        // Probe whether Manager is active
        bool isActive() const;

        // Iterate
        void iterate();

    private:

        /**** Member functions ****/
        // Do idle stuff
        void doIdleStuff();

        // Do busy stuff
        void doBusyStuff();

        // Create Worker
        void createWorker(const std::string& input_string);

        // Terminate Worker
        void terminateWorker();

        // Flush Worker
        void flushWorker();

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

        // Send error code to Master
        void sendErrorCodeToMaster(int error_code);

        /**** Member variables ****/
        // Initial state is idle
        state_t m_state = idle;

        // Command for Worker
        const Command m_command;

        // Worker type (forked Worker vs MPI Worker)
        const worker_t m_worker_type;

        // Pointer to program terminated flag
        bool *m_p_program_terminated;

        // Pointer to Worker handler
        std::unique_ptr<AbstractWorkerHandler> m_p_worker_handler;

        // Message buffer
        std::string m_message_buffer;

        // Message request
        MPI_Request m_message_request = MPI_REQUEST_NULL;

        // Signal buffer
        int m_signal_buffer;

        // Signal request
        MPI_Request m_signal_request = MPI_REQUEST_NULL;

        // Error code buffer
        int m_error_code_buffer;

        // Error code request
        MPI_Request m_error_code_request = MPI_REQUEST_NULL;
};

#endif // MANAGER_H
