#ifndef ABSTRACTMASTER_H
#define ABSTRACTMASTER_H

#include <memory>
#include <string>

#include "AbstractController.h"

class AbstractController;

class AbstractMaster
{
    public:

        // Forward declaration of TaskHandler
        class TaskHandler;

        // Construct from pointer to program terminated flag
        AbstractMaster(bool *p_program_terminated);

        // Virtual destructor
        virtual ~AbstractMaster() = default;

        // Assign controller
        void assignController(
                std::shared_ptr<AbstractController> p_controller);

        // Probe whether Master is active
        virtual bool isActive() const = 0;

        // Iterate
        virtual void iterate() = 0;

        // Returns true if more pending tasks are needed
        virtual bool needMorePendingTasks() const = 0;

        // Push pending task
        virtual void pushPendingTask(const std::string& input_string) = 0;

        // Returns whether finished tasks queue is empty
        virtual bool finishedTasksEmpty() const = 0;

        // Returns reference to front finished task
        virtual TaskHandler& frontFinishedTask() = 0;

        // Pop finished task
        virtual void popFinishedTask() = 0;

        // Flush finished, busy and pending tasks
        virtual void flush() = 0;

        // Terminate Master
        virtual void terminate() = 0;

    protected:

        /**** Member functions ****/
        // Getter for m_p_program_terminated
        bool programTerminated() const;

        /**** Member variables ****/
        // Weak pointer to AbstractController
        std::weak_ptr<AbstractController> m_p_controller;

    private:

        /**** Member variables ****/
        // Pointer to program terminated flag
        bool *m_p_program_terminated;

    public:

        // Nested class for representing tasks
        class TaskHandler
        {
            public:

                // Enumerate type for TaskHandler states
                enum state_t { pending, finished, error };

                // Construct from input string
                TaskHandler(const std::string& input_string);

                // Move constructor
                TaskHandler(TaskHandler &&t);

                // Destructor
                ~TaskHandler();

                // Get state
                state_t getState() const;

                // Probe whether task is pending
                bool isPending() const;

                // Probe whether task is finished
                bool isFinished() const;

                // Probe whether error occured
                bool didErrorOccur() const;

                // Get input string
                std::string getInputString() const;

                // Get output string
                std::string getOutputString() const;

                // Record output
                void recordOutput(const std::string& output_string);

                // Record error
                void recordError();

            private:

                // Initial state is pending
                state_t m_state = pending;

                // Input string
                const std::string m_input_string;

                // Output string, only valid in finished state
                std::string m_output_string;
        };
};

#endif // ABSTRACTMASTER_H
