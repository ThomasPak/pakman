#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <string>

/** A class for representing tasks.
 *
 * A task represents a simulation job, which consists of spawning a
 * simulator, feeding it some input and retrieving the output.
 */
class TaskHandler
{
    public:

        /** Enumeration type for TaskHandler states. */
        enum state_t { pending, finished };

        /** Construct from input string.
         *
         * @param input_string  input string to simulator.
         */
        TaskHandler(const std::string& input_string);

        /** Move constructor. */
        TaskHandler(TaskHandler &&t);

        /** Default destructor does nothing. */
        ~TaskHandler() = default;

        /** @return state of TaskHandler. */
        state_t getState() const;

        /** @return whether task is pending. */
        bool isPending() const;

        /** @return whether task is finished. */
        bool isFinished() const;

        /** @return whether error occurred. */
        bool didErrorOccur() const;

        /** @return error code that simulation job returned. */
        int getErrorCode() const;

        /** @return input string. */
        std::string getInputString() const;

        /** @return output string. */
        std::string getOutputString() const;

        /** Record output and error code.
         *
         * @param output_string  the output string that the simulation
         * job returned.
         * @param error_code the error code that the simulation job
         * returned.
         */
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
        int m_error_code = -1;
};

#endif // TASKHANDLER_H
