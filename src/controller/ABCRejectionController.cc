#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cassert>

#include "core/common.h"
#include "core/types.h"
#include "core/Parameter.h"
#include "interface/protocols.h"
#include "interface/write_parameters.h"
#include "master/AbstractMaster.h"

#include "Sampler.h"

#include "ABCRejectionController.h"

// Constructor
ABCRejectionController::ABCRejectionController(
        const rejection::input_t& input_obj, int num_accept) :
    m_epsilon(input_obj.epsilon),
    m_prior_sampler(input_obj.prior_sampler),
    m_parameter_names(input_obj.parameter_names),
    m_num_accept(num_accept),
    m_simulator(input_obj.simulator)
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
        if (task.didErrorOccur())
        {
            std::runtime_error e("Task finished with error!");
            throw e;
        }

        // Check if parameter was accepted
        if (parse_simulator_output(task.getOutputString()))
        {
            // Declare raw parameter
            std::string raw_parameter;

            // Get input string
            std::stringstream input_sstrm(task.getInputString());

            // Discard epsilon
            std::getline(input_sstrm, raw_parameter);

            // Read accepted parameter
            std::getline(input_sstrm, raw_parameter);

            // Push accepted parameter
            m_prmtr_accepted.push_back(std::move(raw_parameter));
        }

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
        entered = false;
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(format_simulator_input(m_epsilon,
                    m_prior_sampler.sampleParameter()));

    entered = false;
}

Command ABCRejectionController::getSimulator() const
{
    return m_simulator;
}
