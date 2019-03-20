#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cassert>

#include "interface/write_parameters.h"
#include "interface/protocols.h"
#include "core/types.h"
#include "system/system_call.h"
#include "core/utils.h"

#include "SweepController.h"

SweepController::SweepController(const sweep::input_t &input_obj) :
    m_parameter_names(input_obj.parameter_names),
    m_simulator(input_obj.simulator)
{
    // Read output from generator
    std::string generator_output;
    system_call(input_obj.generator, generator_output);

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
    static bool entered = false;
    if (entered) throw;
    entered = true;

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
        entered = false;
        return;
    }

    // Check if there are any new finished parameters
    while (!m_p_master->finishedTasksEmpty())
    {
        // Increment counter
        m_num_finished++;

        // Get reference to front finished task
        AbstractMaster::TaskHandler& task = m_p_master->frontFinishedTask();

        // Do not accept any errors for now
        if (task.didErrorOccur())
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
        write_parameters(std::cout, m_parameter_names, m_prmtr_list);

        // Terminate Master
        m_p_master->terminate();
        entered = false;
        return;
    }

    entered = false;
}

cmd_t SweepController::getSimulator() const
{
    return m_simulator;
}
