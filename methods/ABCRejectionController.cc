#include <iostream>
#include <cassert>

#include "common.h"
#include "types.h"
#include "write_parameters.h"
#include "run_simulation.h"
#include "Sampler.h"
#include "AbstractMaster.h"
#include "ABCRejectionController.h"

// Constructor
ABCRejectionController::ABCRejectionController(
        const rejection::input_t& input_obj, int num_accept) :
    m_epsilon(input_obj.epsilon),
    m_prior_sampler(input_obj.prior_sampler),
    m_parameter_names(input_obj.parameter_names),
    m_num_accept(num_accept)
{
}

// Iterate function
void ABCRejectionController::iterate()
{
    // This function should never be called recursively
    static bool entered = false;
    if (entered) throw;
    entered = true;

    // Check if there are any new accepted parameters
    while (!m_p_master->finishedTasksEmpty())
    {
        // Get reference to front finished task
        AbstractMaster::TaskHandler& task = m_p_master->frontFinishedTask();

        // Do not accept any errors for now
        assert(!task.didErrorOccur());

        // Check if parameter was accepted
        if (simulation_result(task.getOutputString()) == ACCEPT)
        {
            // Get string
            std::string input_string = task.getInputString();

            // Trim epsilon
            input_string.erase(0, 2);

            // Push accepted parameter
            m_prmtr_accepted.push_back(std::move(input_string));
        }

#ifndef NDEBUG
        std::cerr << "task.getInputString(): " << task.getInputString() <<
            std::endl;
#endif

        // Pop finished task
        m_p_master->popFinishedTask();
    }

    // If enough parameters have been accepted, print them and terminate Master
    // and Managers.
    if (m_prmtr_accepted.size() >= m_num_accept)
    {
        // Trim any superfluous parameters
        while (m_prmtr_accepted.size() > m_num_accept)
            m_prmtr_accepted.pop_back();

        // Print accepted parameters
        write_parameters(std::cout, m_parameter_names, m_prmtr_accepted);

        // Terminate Master
        m_p_master->terminate();
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(simulator_input(m_epsilon,
                    m_prior_sampler.sampleParameter()));

    entered = false;
}
