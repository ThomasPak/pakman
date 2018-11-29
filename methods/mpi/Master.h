#ifndef MASTER_H
#define MASTER_H

#include <queue>
#include <vector>
#include <set>
#include <string>
#include <cassert>

#include "../types.h"
#include "../Sampler.h"
#include "ParameterHandler.h"

#include "common.h"

class AbstractMaster
{
    public:

        // Enumerate type for Master states
        enum state_t { normal, flushing, terminated };

        // Construct from pointer to program terminated flag
        AbstractMaster(bool *p_program_terminated) :
            m_comm_size(MPI::COMM_WORLD.Get_size()),
            m_p_program_terminated(p_program_terminated),
            m_map_manager_to_task(MPI::COMM_WORLD.Get_size()),
            m_message_buffers(MPI::COMM_WORLD.Get_size()),
            m_message_requests(MPI::COMM_WORLD.Get_size()),
            m_signal_requests(MPI::COMM_WORLD.Get_size())
        {
            for (int i = 0; i < m_comm_size; i++)
                m_idle_managers.insert(i);
        }

        // Destructor
        ~AbstractMaster() = default;

        // Get state of Master
        state_t getState() const
        {
            return m_state;
        }

        // Probe whether Master is active
        bool isActive() const
        {
            return m_state != terminated;
        }

        // Iterate
        void iterate()
        {
            // This function should never be called recursively
            static bool entered = false;
            if (entered) throw;
            entered = true;

            // This function should never be called if the Master has
            // terminated
            assert(m_state != terminated);

            // Switch based on state
            switch (m_state)
            {
                case normal:
                    doNormalStuff();
                    break;

                case flushing:
                    doFlushingStuff();
                    break;

                default:
                    throw;
            }

            entered = false;
        }

    protected:

        // Class for representing tasks
        class TaskHandler
        {
            public:

                // Enumerate type for TaskHandler states
                enum state_t { pending, finished, error };

                // Construct from input string
                TaskHandler(const std::string& input_string) :
                    m_input_string(input_string)
                {
#ifndef NDEBUG
                    std::cerr << "DEBUG: TaskHandler constructor!\n";
#endif
                }

                // Move constructor
                TaskHandler(TaskHandler &&t) :
                    m_input_string(std::move(t.m_input_string)),
                    m_output_string(std::move(t.m_output_string)),
                    m_state(t.m_state)
                {
#ifndef NDEBUG
                    std::cerr << "DEBUG: TaskHandler move constructor!\n";
#endif
                }

                // Destructor
                ~TaskHandler()
                {
#ifndef NDEBUG
                    std::cerr << "DEBUG: TaskHandler destructor!\n";
#endif
                }

                // Get state
                state_t getState() const
                {
                    return m_state;
                }

                // Probe whether task is pending
                bool isPending() const
                {
                    return m_state == pending;
                }

                // Probe whether task is finished
                bool isFinished() const
                {
                    return m_state == finished;
                }

                // Probe whether error occured
                bool didErrorOccur() const
                {
                    return m_state == error;
                }

                // Get input string
                std::string getInputString() const
                {
                    // Return input string
                    return m_input_string;
                }

                // Get output string
                std::string getOutputString() const
                {
                    // This should only be called in the finished state
                    assert(m_state == finished);

                    // Return output string
                    return m_output_string;
                }

                // Record output
                void recordOutput(const std::string& output_string)
                {
                    // This should only be called in the pending state
                    assert(m_state == pending);

                    // Record output string and set state to finished
                    m_output_string.assign(output_string);
                    m_state = finished;
                }

                // Record error
                void recordError()
                {
                    // This should only be called in the pending state
                    assert(m_state == pending);

                    // Set state to finished
                    m_state = error;
                }

            private:

                // Initial state is pending
                state_t m_state = pending;

                // Input string
                const std::string m_input_string;

                // Output string, only valid in finished state
                std::string m_output_string;
        };

        // Number of pending tasks
        int numberOfPendingTasks() const
        {
            return m_pending_tasks.size();
        }

        // Number of finished tasks
        int numberOfFinishedTasks() const
        {
            return m_finished_tasks.size();
        }

        // Queue task
        void queueTask(const std::string& input_string);

        // Pop finished tasks
        void popFinishedTasks(std::vector<TaskHandler>& popped_tasks);

        // Terminate Workers
        void terminateWorkers()
        {
            m_worker_terminated = true;
        }

        // Terminate Master and Managers
        void terminateMasterAndManagers()
        {
            m_master_manager_terminated = true;
        }

        // Dummy processTasks
        virtual void processTasks() = 0;

        // Communicator size
        const int m_comm_size;

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

        // Probe for Manager rank of incoming message
        int probeMessageManager() const;

        // Probe for Manager rank of incoming signal
        int probeSignalManager() const;

        // Receive message from Manager
        std::string receiveMessage(int manager_rank) const;

        // Receive signal from Manager
        int receiveSignal(int manager_rank) const;

        // Send message to a Manager
        void sendMessageToManager(int manager_rank,
                const std::string& message_string);

        // Send signal to all Managers
        void sendSignalToAllManagers(int signal);

        /**** Member variables ****/
        // Initial state is normal
        state_t m_state = normal;

        // Pointer to program terminated flag
        bool *m_p_program_terminated;

        // Flag for terminating Master and Managers
        bool m_master_manager_terminated = false;

        // Flag for terminating Workers
        bool m_worker_terminated = false;

        // Set of idle managers
        std::set<int> m_idle_managers;

        // Mapping from Manager to corresponding task
        std::vector<TaskHandler*> m_map_manager_to_task;

        // Finished tasks
        std::vector<TaskHandler> m_finished_tasks;

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

class ABCRejectionMaster : public AbstractMaster
{
    public:

        // Constructor
        ABCRejectionMaster(const rejection::input_t& input_obj,
                int num_accept, bool *p_program_terminated) :
            AbstractMaster(p_program_terminated),
            m_epsilon(input_obj.epsilon),
            m_prior_sampler(input_obj.prior_sampler),
            m_parameter_names(input_obj.parameter_names),
            m_num_accept(num_accept)
        {
        }

    protected:

        // Process tasks
        virtual void processTasks() override;

    private:

        /**** Member functions ****/
        void printAcceptedParameters();

        /**** Member variables ****/
        // Epsilon
        std::string m_epsilon;

        // Prior sampler
        PriorSampler m_prior_sampler;

        // Parameter names
        std::vector<std::string> m_parameter_names;

        // Number of parameter to accept
        int m_num_accept;

        // Vector of accepted parameters
        std::vector<parameter_t> m_prmtr_accepted;
};

void check_managers(const std::vector<ParameterHandler*>& manager_map,
                    std::set<int>& idle_managers);
void delegate_managers(const AbstractSampler& sampler_obj,
                       const std::string& epsilon,
                       std::vector<ParameterHandler*>& manager_map,
                       std::set<int>& idle_managers,
                       std::queue<ParameterHandler>& prmtr_sampled);
void check_parameters(std::queue<ParameterHandler>& prmtr_sampled,
                      std::vector<parameter_t>& prmtr_accepted);
void send_signal_to_managers(const int signal);

namespace rejection {

void master(const int num_accept, const input_t& input_obj);

}

namespace mcmc {

void master();

}

namespace smc {

void compute_weights(const SMCSampler& smc_sampler,
                     const cmd_t& perturbation_pdf,
                     const int t,
                     const std::vector<parameter_t>::const_iterator& it_end,
                     std::vector<parameter_t>::const_iterator& it,
                     std::vector<double>& weights_new);
void master(const int num_accept, const input_t& input_obj);

}

namespace sweep {

void master(const input_t& input_obj);

}

#endif // MASTER_H
