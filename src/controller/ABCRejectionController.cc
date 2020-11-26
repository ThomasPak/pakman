#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <assert.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "core/OutputStreamHandler.h"
#include "interface/protocols.h"
#include "interface/output.h"
#include "master/AbstractMaster.h"

#include "ABCRejectionController.h"

// Constructor
ABCRejectionController::ABCRejectionController(const Input& input_obj) :
    m_number_accept(input_obj.number_accept),
    m_epsilon(input_obj.epsilon),
    m_prior_sampler(input_obj.prior_sampler),
    m_parameter_names(input_obj.parameter_names),
    m_simulator(input_obj.simulator)
{
}

// Iterate function
void ABCRejectionController::iterate()
{
    // This function should never be called recursively
    assert(!m_entered);
    m_entered = true;

    // Check if there are any new accepted parameters
    while (!m_p_master->finishedTasksEmpty()
            && m_prmtr_accepted.size() < m_number_accept)
    {
        // Increment counter
        m_number_simulated++;

        // Get reference to front finished task
        TaskHandler& task = m_p_master->frontFinishedTask();

        // Check if error occured
        if (!task.didErrorOccur())
        {
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
        }
        // If error occurred, check if g_ignore_errors is set
        else if (!g_ignore_errors)
        {
            std::runtime_error e("Task finished with error!");
            throw e;
        }

        // Pop finished task
        m_p_master->popFinishedTask();
    }

    // If enough parameters have been accepted, print them and terminate Master
    // and Managers.
    if (m_prmtr_accepted.size() == m_number_accept)
    {
        // Print message
        spdlog::info("Accepted/simulated: {}/{} ({:5.2f}%)",
                m_number_accept, m_number_simulated, (100.0 * m_number_accept /
                    (double) m_number_simulated));

        // Print accepted parameters
        write_parameters(OutputStreamHandler::instance()->getOutputStream(),
                m_parameter_names, m_prmtr_accepted);

        // Terminate Master
        m_p_master->terminate();
        m_entered = false;
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(format_simulator_input(m_epsilon.str(),
                    sample_from_prior(m_prior_sampler)));

    m_entered = false;
}

Command ABCRejectionController::getSimulator() const
{
    return m_simulator;
}
