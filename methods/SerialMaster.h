#ifndef SERIALMASTER_H
#define SERIALMASTER_H

#include <string>
#include <queue>

#include "AbstractMaster.h"

class SerialMaster : public AbstractMaster
{
    public:

        // Enumerate type for SerialMaster states
        enum state_t { normal, terminated };

        // Construct from pointer to program terminated flag
        SerialMaster(const cmd_t& command, bool *p_program_terminated);

        // Destructor
        virtual ~SerialMaster() override = default;

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

        // Flush finished pending tasks
        virtual void flush() override;

        // Terminate Master
        virtual void terminate() override;

    private:

        /**** Member functions ****/
        // Processes a task from pending queue if there is one and places it in
        // the finished queue when done.
        void processTask();

        /**** Member variables ****/
        // Initial state is normal
        state_t m_state = normal;

        // Simulator command
        const cmd_t m_command;

        // Finished tasks
        std::queue<TaskHandler> m_finished_tasks;

        // Pending tasks
        std::queue<TaskHandler> m_pending_tasks;

};

#endif // SERIALMASTER_H
