#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <assert.h>

#include "core/utils.h"
#include "core/OutputStreamHandler.h"
#include "system/system_call.h"
#include "interface/output.h"
#include "interface/protocols.h"
#include "master/AbstractMaster.h"

#include "SweepController.h"

SweepController::SweepController(const Input &input_obj) :
    m_parameter_names(input_obj.parameter_names),
    m_simulator(input_obj.simulator)
{
    // Read output from generator
    std::string generator_output = system_call(input_obj.generator);

    // Decompose into individual parameters
    m_prmtr_list = parse_generator_output(generator_output);

    // Sanity check: at least one parameter should have been generated
    if (m_prmtr_list.size() == 0)
    {
        std::runtime_error e("generator did not output any parameters");
        throw e;
    }
}

void SweepController::iterate()
{
    // This function should never be called recursively
    assert(!m_entered);
    m_entered = true;

    // If in the first iteration
    if (m_first_iteration)
    {
        // Unset flag
        m_first_iteration = false;

        // Push all parameters
        for (auto it = m_prmtr_list.begin(); it != m_prmtr_list.end(); it++)
        {
            std::string input(it->str());
            input += '\n';
            m_p_master->pushPendingTask(std::move(input));
        }

        // Return
        m_entered = false;
        return;
    }

    // Check if there are any new finished parameters
    while (!m_p_master->finishedTasksEmpty())
    {
        // Increment counter
        m_num_finished++;

        // Get reference to front finished task
        TaskHandler& task = m_p_master->frontFinishedTask();

        // Throw error if task finished with error and we are not ignoring
        // task errors
        if (!g_ignore_errors && task.didErrorOccur())
        {
            std::runtime_error e("Task finished with error!");
            throw e;
        }

        // Pop finished parameters
        m_p_master->popFinishedTask();
    }

    // If all parameters have finished, then write parameters and terminate
    // Master
    if (m_num_finished == m_prmtr_list.size())
    {
        // Print finished parameters
        write_parameters(OutputStreamHandler::instance()->getOutputStream(),
                m_parameter_names, m_prmtr_list);

        // Terminate Master
        m_p_master->terminate();
        m_entered = false;
        return;
    }

    m_entered = false;
}

Command SweepController::getSimulator() const
{
    return m_simulator;
}
