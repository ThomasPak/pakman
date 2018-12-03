#include <memory>
#include <string>
#include <cassert>

#include "AbstractMaster.h"

// Construct from pointer to program terminated flag
AbstractMaster::AbstractMaster(bool *p_program_terminated) :
    m_p_program_terminated(p_program_terminated)
{
}

// Assign controller
void AbstractMaster::assignController(
        std::shared_ptr<AbstractController> p_controller)
{
    m_p_controller = p_controller;
}

// Getter for m_p_program_terminated
bool AbstractMaster::programTerminated() const
{
    return *m_p_program_terminated;
}

// Construct from input string
AbstractMaster::TaskHandler::TaskHandler(const std::string& input_string) :
    m_input_string(input_string)
{
}

// Move constructor
AbstractMaster::TaskHandler::TaskHandler(TaskHandler &&t) :
    m_input_string(std::move(t.m_input_string)),
    m_output_string(std::move(t.m_output_string)),
    m_state(t.m_state)
{
}

// Destructor
AbstractMaster::TaskHandler::~TaskHandler()
{
}

// Get state
AbstractMaster::TaskHandler::state_t
AbstractMaster::TaskHandler::getState() const
{
    return m_state;
}

// Probe whether task is pending
bool AbstractMaster::TaskHandler::isPending() const
{
    return m_state == pending;
}

// Probe whether task is finished
bool AbstractMaster::TaskHandler::isFinished() const
{
    return m_state == finished;
}

// Probe whether error occured
bool AbstractMaster::TaskHandler::didErrorOccur() const
{
    return m_state == error;
}

// Get input string
std::string AbstractMaster::TaskHandler::getInputString() const
{
    // Return input string
    return m_input_string;
}

// Get output string
std::string AbstractMaster::TaskHandler::getOutputString() const
{
    // This should only be called in the finished state
    assert(m_state == finished);

    // Return output string
    return m_output_string;
}

// Record output
void AbstractMaster::TaskHandler::recordOutput(const std::string& output_string)
{
    // This should only be called in the pending state
    assert(m_state == pending);

    // Record output string and set state to finished
    m_output_string.assign(output_string);
    m_state = finished;
}

// Record error
void AbstractMaster::TaskHandler::recordError()
{
    // This should only be called in the pending state
    assert(m_state == pending);

    // Set state to finished
    m_state = error;
}
