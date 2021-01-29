#include <assert.h>

#include "TaskHandler.h"

// Construct from input string
TaskHandler::TaskHandler(const std::string& input_string) :
    m_input_string(input_string)
{
}

// Move constructor
TaskHandler::TaskHandler(TaskHandler &&t) :
    m_input_string(std::move(t.m_input_string)),
    m_output_string(std::move(t.m_output_string)),
    m_error_code(t.m_error_code),
    m_state(t.m_state)
{
}

// Get state
TaskHandler::state_t TaskHandler::getState() const
{
    return m_state;
}

// Probe whether task is pending
bool TaskHandler::isPending() const
{
    return m_state == pending;
}

// Probe whether task is finished
bool TaskHandler::isFinished() const
{
    return m_state == finished;
}

// Probe whether error occured
bool TaskHandler::didErrorOccur() const
{
    // This should only be called in the finished state
    assert(m_state == finished);

    return m_error_code != 0;
}

// Get input string
std::string TaskHandler::getInputString() const
{
    // Return input string
    return m_input_string;
}

// Get output string
std::string TaskHandler::getOutputString() const
{
    // Return output string
    return m_output_string;
}

// Get error code
int TaskHandler::getErrorCode() const
{
    // Return error code
    return m_error_code;
}

// Record output
void TaskHandler::recordOutputAndErrorCode(const std::string& output_string,
        int error_code)
{
    // This should only be called in the pending state
    assert(m_state == pending);

    // Record output string, error code and set state to finished
    m_output_string.assign(output_string);
    m_error_code = error_code;
    m_state = finished;
}
