#ifndef ABSTRACTMASTER_H
#define ABSTRACTMASTER_H

#include <memory>
#include <string>

#include "common.h"
#include "LongOptions.h"
#include "Arguments.h"

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

        // Return master type based on string
        static master_t getMaster(const std::string& arg);

        // Return help message based on master type.
        // When subclassing AbstractMaster, be sure to include a static method
        // with the signature
        //
        // 'static std::string help()'
        //
        // and add an entry in
        // the switch statement of AbstractMaster::help()
        static std::string help(master_t master);

        // Add Master-specific long options to the given LongOptions object.
        // When subclassing AbstractMaster, be sure to include a static method
        // with the signature
        //
        // 'static void addLongOptions(LongOptions& lopts)'
        //
        // and add an entry in
        // the switch statement of AbstractMaster::addLongOptions()
        static void addLongOptions(master_t master, LongOptions& lopts);

        // Execute run function based on master type
        // When subclassing AbstractMaster, be sure to include a static method
        // with the signature
        //
        // 'static void run(controller_t controller, int argc, char *argv[])'
        //
        // and add an entry in the switch statement of AbstractMaster::run()
        static void run(master_t master, controller_t controller,
                const Arguments& args);

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
                enum state_t { pending, finished };

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

                // Probe whether error occurred
                bool didErrorOccur() const;

                // Get error code
                int getErrorCode() const;

                // Get input string
                std::string getInputString() const;

                // Get output string
                std::string getOutputString() const;

                // Record output and error code
                void recordOutputAndErrorCode(const std::string& output_string,
                        int error_code);

            private:

                // Initial state is pending
                state_t m_state = pending;

                // Input string
                const std::string m_input_string;

                // Output string, only valid in finished state
                std::string m_output_string;

                // Error code, only valid in finished state
                int m_error_code;
        };
};

#endif // ABSTRACTMASTER_H
