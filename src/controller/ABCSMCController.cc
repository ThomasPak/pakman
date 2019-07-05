#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <random>

#include <assert.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "interface/types.h"
#include "interface/protocols.h"
#include "interface/write_parameters.h"

#include "smc_weight.h"

#include "ABCSMCController.h"

// Constructor
ABCSMCController::ABCSMCController(const Input &input_obj,
        std::shared_ptr<std::default_random_engine> p_generator) :
    m_epsilons(input_obj.epsilons),
    m_parameter_names(input_obj.parameter_names),
    m_smc_sampler(std::vector<double>(input_obj.population_size),
            std::vector<Parameter>(input_obj.population_size), p_generator, input_obj.perturber,
            input_obj.prior_sampler, input_obj.prior_pdf),
    m_perturbation_pdf(input_obj.perturbation_pdf),
    m_population_size(input_obj.population_size),
    m_simulator(input_obj.simulator)
{
    m_smc_sampler.setT(m_t);
}

// Iterate function
void ABCSMCController::iterate()
{
    // This function should never be called recursively
    static bool entered = false;
    if (entered) throw;
    entered = true;

    // Display message if first iteration
    static bool first = true;
    if (first)
    {
        spdlog::info("Computing generation {}, epsilon = {}", m_t,
                m_epsilons[m_t].str());
        first = false;
    }

    // If m_t is equal to the number of epsilons, something went wrong because
    // the algorithm should have finished
    assert(m_t < m_epsilons.size());

    // Check if there are any new accepted parameters
    while (!m_p_master->finishedTasksEmpty()
            && m_prmtr_accepted_new.size() < m_population_size)
    {
        // Increment counter
        m_number_simulated++;

        // Get reference to front finished task
        AbstractMaster::TaskHandler& task = m_p_master->frontFinishedTask();

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
                m_prmtr_accepted_new.push_back(std::move(raw_parameter));
            }
        }
        // If error occurred, check if ignore_errors is set
        else if (!ignore_errors)
        {
            std::runtime_error e("Task finished with error!");
            throw e;
        }

        // Pop finished task
        m_p_master->popFinishedTask();
    }

    // Iterate over parameters whose weights have not yet been computed
    for (int i = m_weights_new.size(); i < m_prmtr_accepted_new.size(); i++)
        m_weights_new.push_back(smc_weight(
                m_perturbation_pdf,
                m_smc_sampler.getPriorPdf(),
                m_t,
                m_smc_sampler.getParameterPopulation(),
                m_smc_sampler.getWeights(),
                m_prmtr_accepted_new[i]));

    // If enough parameters have been accepted for this generation, check if we
    // are in the last generation.  If we are in the last generation, then
    // print the accepted parameters and terminate Master.  If we are not in
    // the last generation, then swap the weights and populations
    if (m_prmtr_accepted_new.size() == m_population_size)
    {
        // Print message
        spdlog::info("Accepted/simulated: {}/{} ({:5.2f}%)",
                m_population_size, m_number_simulated, (100.0 * m_population_size /
                    (double) m_number_simulated));
        m_number_simulated = 0;

        // Increment generation counter
        m_t++;
        m_smc_sampler.setT(m_t);

        // Check if we are in the last generation
        if (m_t == m_epsilons.size())
        {
            // Print accepted parameters
            write_parameters(std::cout, m_parameter_names, m_prmtr_accepted_new);

            // Terminate Master
            m_p_master->terminate();
            entered = false;
            return;
        }

        // Swap population and weights
        m_smc_sampler.swap_population(m_weights_new, m_prmtr_accepted_new);

        // Clear m_weights_new and m_prmtr_accepted_new
        m_weights_new.clear();
        m_prmtr_accepted_new.clear();

        // Flush Master
        m_p_master->flush();
        entered = false;

        // Print message
        spdlog::info("Computing generation {}, epsilon = {}", m_t,
                m_epsilons[m_t].str());
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(
                format_simulator_input(m_epsilons[m_t].str(),
                    m_smc_sampler.sampleParameter()));

    entered = false;
}

Command ABCSMCController::getSimulator() const
{
    return m_simulator;
}
