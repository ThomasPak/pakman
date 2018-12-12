#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "write_parameters.h"
#include "types.h"
#include "system_call.h"
#include "vector_strtok.h"

#include "SweepController.h"

SweepController::SweepController(const sweep::input_t &input_obj) :
    m_parameter_names(input_obj.parameter_names)
{
    // Read output from generator
    std::string generator_output;
    system_call(input_obj.generator, generator_output);

    // Decompose into individual parameters
    vector_strtok(generator_output, m_prmtr_list, "\n");

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
            m_p_master->pushPendingTask(*it);

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
        assert(!task.didErrorOccur());

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
