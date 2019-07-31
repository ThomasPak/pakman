#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <memory>

#include <assert.h>

#include <mpi.h>

#include "core/Command.h"

class AbstractWorkerHandler;

/** A helper class for performing simulation tasks in parallel using MPI.
 *
 * The role of Managers is to accept simulation tasks from the MPIMaster, spawn
 * child processes (also called Workers) to perform the simulation, and report
 * the results back to MPIMaster.
 *
 * The Workers can be either a forked Worker or an MPI Worker.  These are
 * represented by the ForkedWorkerHandler and MPIWorkerHandler classes,
 * respectively (both are derived form the AbstractWorkerHandler class).  The
 * MPI Worker is necessary when the simulator uses MPI.  See MPIMaster for more
 * details.
 *
 * As with the MPIMaster, Managers are meant to be run in an event loop.
 * Therefore, the event loop in MPIMaster::run() will call Manager::iterate().
 */

class Manager
{
    public:

        /** Enumerate type for Worker types. */
        enum worker_t
        {
            forked_worker,
            mpi_worker
        };

        /** Constructor.
         *
         * @param simulator  command to run simulation.
         * @param worker_type  type of Worker
         * @param p_program_terminated  pointer to boolean flag that is set
         * when the execution of Pakman is terminated by the user.
         */
        Manager(const Command &simulator, worker_t worker_type,
                bool *p_program_terminated);

        /** Default destructor destroys MPI_Request objects. */
        ~Manager();

        /** @return whether the Manager is active. */
        bool isActive() const;

        /** Iterates the Manager in an event loop. */
        void iterate();

    private:

        /** Enumerate type for Manager states.
         *
         * The Manager can either in a `idle` state, a `busy` state, or in a
         * `terminated` state.  When the Manager is in a `terminated` state,
         * the member function isActive() will return false and the event loop
         * should terminate.
         */
        enum state_t { idle, busy, terminated };

        ///// Member functions /////
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

        ///// Member variables /////
        // Initial state is idle
        state_t m_state = idle;

        // Command for Worker
        const Command m_simulator;

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
