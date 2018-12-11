#ifndef MPIMASTER_H
#define MPIMASTER_H

#include <queue>
#include <vector>
#include <set>
#include <string>

#include "../AbstractMaster.h"

#include "mpi_common.h"

class MPIMaster : public AbstractMaster
{
    public:

        // Enumerate type for Master states
        enum state_t { normal, flushing, terminated };

        // Construct from pointer to program terminated flag
        MPIMaster(bool *p_program_terminated);

        // Destructor
        virtual ~MPIMaster() override = default;

        // Probe whether Master is active
        virtual bool isActive() const override;

        // Iterate
        virtual void iterate() override;

        // Returns true if more pending tasks are needed
        virtual bool needMorePendingTasks() const override;

        // Push pending task
        virtual void pushPendingTask(const std::string& input_string) override;

        // Returns whether finished tasks queue is empty
        virtual bool finishedTasksEmpty() const override;

        // Returns reference to front finished task
        virtual TaskHandler& frontFinishedTask() override;

        // Pop finished task
        virtual void popFinishedTask() override;

        // Flush finished, busy and pending tasks
        virtual void flush() override;

        // Terminate Master
        virtual void terminate() override;

    private:

        /**** Member functions ****/
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

        // Discard any messages and signals until all Managers are idle
        void discardMessagesAndSignals();

        // Probe for message
        bool probeMessage() const;

        // Probe for signal
        bool probeSignal() const;

        // Probe for error code
        bool probeErrorCode() const;

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

        /**** Member variables ****/
        // Initial state is normal
        state_t m_state = normal;

        // Communicator size
        const int m_comm_size;

        // Flag for terminating Master and Managers
        bool m_master_manager_terminated = false;

        // Flag for terminating Workers
        bool m_worker_terminated = false;

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
        std::vector<MPI::Request> m_message_requests;

        // Signal buffer (assumption: every signal goes to all Managers, so
        // only one signal buffer is required)
        int m_signal_buffer;

        // Signal requests
        std::vector<MPI::Request> m_signal_requests;
};

#endif // MPIMASTER_H
