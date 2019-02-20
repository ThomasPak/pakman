#include <cassert>
#include <memory>
#include <string>

#include "common.h"
#include "types.h"
#include "system_call.h"
#include "help.h"

#include "AbstractMaster.h"
#include "SerialMaster.h"

// Construct from pointer to program terminated flag
SerialMaster::SerialMaster(const cmd_t& command, bool *p_program_terminated) :
    AbstractMaster(p_program_terminated), m_command(command)
{
}

// Probe whether Master is active
bool SerialMaster::isActive() const
{
    return m_state != terminated;
}

// Iterate
void SerialMaster::iterate()
{
    // This function should never be called recursively
    static bool entered = false;
    if (entered) throw;
    entered = true;

    // This function should never be called if the Master has
    // terminated
    assert(m_state != terminated);

    // Check for program termination interrupt
    if (programTerminated())
    {
        // Terminate Master
        m_state = terminated;
        return;
    }

    // If there is at least one pending task, execute the task
    processTask();

    // Call controller
    if (std::shared_ptr<AbstractController> p_controller =
            m_p_controller.lock())
        p_controller->iterate();

    entered = false;
}

// Returns true if more pending tasks are needed
bool SerialMaster::needMorePendingTasks() const
{
    return m_pending_tasks.size() < 1;
}

// Push pending task
void SerialMaster::pushPendingTask(const std::string& input_string)
{
    m_pending_tasks.push(input_string);
}

// Returns whether finished tasks queue is empty
bool SerialMaster::finishedTasksEmpty() const
{
    return m_finished_tasks.empty();
}

// Returns reference to front finished task
AbstractMaster::TaskHandler& SerialMaster::frontFinishedTask()
{
    return m_finished_tasks.front();
}

// Pop finished task
void SerialMaster::popFinishedTask()
{
    m_finished_tasks.pop();
}

// Flush finished and pending tasks
void SerialMaster::flush()
{
    while (!m_finished_tasks.empty()) m_finished_tasks.pop();
    while (!m_pending_tasks.empty()) m_pending_tasks.pop();
}

// Terminate Master
void SerialMaster::terminate()
{
    m_state = terminated;
    return;
}

// Processes a task from pending queue if there is one and places it in
// the finished queue when done.
void SerialMaster::processTask()
{
    // Return immediately if there are no pending tasks
    if (m_pending_tasks.empty())
        return;

    // Else, pop a pending task, process it and push it to the finished queue
    TaskHandler& current_task = m_pending_tasks.front();

    // Process current task and get output string and error code
    std::string output_string;
    int error_code;
    system_call(m_command, current_task.getInputString(), output_string, error_code);

    // Record output string and error code
    current_task.recordOutputAndErrorCode(output_string, error_code);

    // Move task to finished queue
    m_finished_tasks.push(std::move(m_pending_tasks.front()));

    // Pop pending queue
    m_pending_tasks.pop();
}
