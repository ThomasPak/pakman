#ifndef MPIMASTER_H
#define MPIMASTER_H

#include <queue>
#include <vector>
#include <set>
#include <string>

#include <mpi.h>

#include "core/common.h"

#include "AbstractMaster.h"

class LongOptions;
class Arguments;

/** A Master class for performing simulation tasks in parallel using MPI.
 *
 * The MPIMaster class performs simulation tasks in parallel using MPI by
 * delegating simulation tasks to a pool of Managers (as implemented by the
 * Manager class).  These Managers then perform simulation tasks by spawning
 * child processes with `fork()`--`exec()` to run simulation.
 *
 * @warning If your simulator uses MPI internally, this will likely clash with
 * Pakman when using MPIMaster.  In that case, you will need to build an MPI
 * simulator.  An example of an MPI simulator can be found [on our
 * wiki](
 * https://github.com/ThomasPak/pakman/wiki/Example:-epithelial-cell-growth).
 *
 * For instructions on how to use Pakman with the MPI master, execute the
 * following command
 * ```
 * $ pakman mpi --help
 * ```
 */

class MPIMaster : public AbstractMaster
{
    public:

        /** Constructor saves program termination flag.
         *
         * @param p_program_terminated  pointer to boolean flag that is set
         * when the execution of Pakman is terminated by the user.
         */
        MPIMaster(bool *p_program_terminated);

        /** Default destructor does nothing. */
        virtual ~MPIMaster() override;

        /** @return whether the MPIMaster is active. */
        virtual bool isActive() const override;

        /** @return whether more pending tasks are needed. */
        virtual bool needMorePendingTasks() const override;

        /** Push a new pending task.
         *
         * @param input_string  input string to simulation job.
         */
        virtual void pushPendingTask(const std::string& input_string) override;

        /** @return whether finished tasks queue is empty. */
        virtual bool finishedTasksEmpty() const override;

        /** @return reference to front finished task. */
        virtual TaskHandler& frontFinishedTask() override;

        /** Pop front finished task. */
        virtual void popFinishedTask() override;

        /** Flush all finished, busy and pending tasks. */
        virtual void flush() override;

        /** Terminate MPIMaster. */
        virtual void terminate() override;

        /** @return help message string. */
        static std::string help();

        /** Add long command-line options.
         *
         * @param lopts  long command-line options that the MPIMaster needs.
         */
        static void addLongOptions(LongOptions& lopts);

        /** Run MPIMaster in an event loop.
         *
         * This function creates the MPIMaster and Controller objects, and
         * runs them in an event loop.
         *
         * @param controller  controller type.
         * @param args  command-line arguments.
         */
        static void run(controller_t controller, const Arguments& args);

        /** Terminate all Managers, as well as the MPIWorker associated with
         * rank 0, and terminate MPI.
         *
         * This function is used when an exception occurs in run() and MPI
         * processes need to be cleaned up appropriately so that the program
         * does not hang after an exception.
         */
        static void cleanup();

    protected:

        /** Iterates the MPIMaster in an event loop. */
        virtual void iterate() override;

    private:

        /** Enumerate type for MPIMaster states.
         *
         * The MPIMaster can either in a `normal` state, a `flushing` state or
         * in a `terminated` state.  When the MPIMaster is in a flushing state,
         * it has flushed the task queues and is waiting for all Managers to
         * finish ongoing simulations before accepting new tasks.  When the
         * MPIMaster is in a `terminated` state, the member function isActive()
         * will return false and the event loop should terminate.
         */
        enum state_t { normal, flushing, terminated };

        ///// Member functions /////
        // Do normal stuff
        void doNormalStuff();

        // Do flushing stuff
        void doFlushingStuff();

        // Listen to messages from Managers.
        void listenToManagers();

        // Pop finished tasks from busy queue and insert into finished queue
        void popBusyQueue();

        // Delegate to Managers
        void delegateToManagers();

        // Flush all task queues (finished, busy, pending)
        void flushQueues();

        // Discard any messages, error codes and signals until all Managers are
        // idle
        void discardMessagesErrorCodesAndSignals();

        // Probe for message
        bool probeMessage() const;

        // Probe for signal
        bool probeSignal() const;

        // Probe for Manager rank of incoming message
        int probeMessageManager() const;

        // Probe for Manager rank of incoming signal
        int probeSignalManager() const;

        // Receive message from Manager
        std::string receiveMessage(int manager_rank) const;

        // Receive signal from Manager
        int receiveSignal(int manager_rank) const;

        // Receive error cdoe from Manager
        int receiveErrorCode(int manager_rank) const;

        // Send message to a Manager
        void sendMessageToManager(int manager_rank,
                const std::string& message_string);

        // Send signal to all Managers
        void sendSignalToAllManagers(int signal);

        ///// Member variables /////
        // Initial state is normal
        state_t m_state = normal;

        // Communicator size
        const int m_comm_size;

        // Flag for terminating Master and Managers
        bool m_master_manager_terminated = false;

        // Flag for flushing Workers
        bool m_worker_flushed = false;

        // Set of idle managers
        std::set<int> m_idle_managers;

        // Mapping from Manager to corresponding task
        std::vector<TaskHandler*> m_map_manager_to_task;

        // Finished tasks
        std::queue<TaskHandler> m_finished_tasks;

        // Busy tasks
        std::queue<TaskHandler> m_busy_tasks;

        // Pending tasks
        std::queue<TaskHandler> m_pending_tasks;

        // Message buffers
        std::vector<std::string> m_message_buffers;

        // Message requests
        std::vector<MPI_Request> m_message_requests;

        // Signal buffer (assumption: every signal goes to all Managers, so
        // only one signal buffer is required)
        int m_signal_buffer;

        // Signal requests
        std::vector<MPI_Request> m_signal_requests;

        // Entered iterate()
        bool m_entered = false;
};

#endif // MPIMASTER_H
