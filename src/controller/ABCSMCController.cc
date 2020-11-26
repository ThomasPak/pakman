#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <random>

#include <assert.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "core/OutputStreamHandler.h"
#include "interface/protocols.h"
#include "interface/output.h"
#include "master/AbstractMaster.h"

#include "smc_weight.h"
#include "sample_population.h"

#include "ABCSMCController.h"

// Constructor
ABCSMCController::ABCSMCController(const Input &input_obj) :
    m_epsilons(input_obj.epsilons),
    m_parameter_names(input_obj.parameter_names),
    m_population_size(input_obj.population_size),
    m_simulator(input_obj.simulator),
    m_prior_sampler(input_obj.prior_sampler),
    m_prior_pdf(input_obj.prior_pdf),
    m_perturber(input_obj.perturber),
    m_perturbation_pdf(input_obj.perturbation_pdf),
    m_generator(input_obj.seed),
    m_distribution(0.0, 1.0),
    m_prmtr_accepted_old(input_obj.population_size),
    m_weights_old(input_obj.population_size)
{
}

// Iterate function
void ABCSMCController::iterate()
{
    // This function should never be called recursively
    assert(!m_entered);
    m_entered = true;

    // Display message if first iteration
    if (m_first)
    {
        spdlog::info("Computing generation {}, epsilon = {}", m_t,
                m_epsilons[m_t].str());
        m_first = false;
    }

    // If m_t is equal to the number of epsilons, something went wrong because
    // the algorithm should have finished
    assert(m_t < m_epsilons.size());

    // Check if there are any new accepted parameters
    while (!m_p_master->finishedTasksEmpty()
            && m_prmtr_accepted_new.size() < m_population_size)
    {
        // m_prior_pdf_pending should not be empty
        assert(!m_prior_pdf_pending.empty());

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
                m_prmtr_accepted_new.push_back(std::move(raw_parameter));

                // Push prior_pdf of accepted parameter
                m_prior_pdf_accepted.push_back(m_prior_pdf_pending.front());
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

        // Pop prior_pdf of finished task
        m_prior_pdf_pending.pop();
    }

    // Iterate over parameters whose weights have not yet been computed
    for (int i = m_weights_new.size(); i < m_prmtr_accepted_new.size(); i++)
        m_weights_new.push_back(smc_weight(
                m_perturbation_pdf,
                m_prior_pdf_accepted[i],
                m_t,
                m_prmtr_accepted_old,
                m_weights_old,
                m_prmtr_accepted_new[i]));

    // If enough parameters have been accepted for this generation, check if we
    // are in the last generation.  If we are in the last generation, then
    // print the accepted parameters and terminate Master.  If we are not in
    // the last generation, then swap the weights and populations
    if (m_prmtr_accepted_new.size() == m_population_size)
    {
        // Print message
        spdlog::info("Accepted/simulated: {}/{} ({:5.2f}%)",
                m_population_size, m_number_simulated,
                (100.0 * m_population_size / (double) m_number_simulated));
        m_number_simulated = 0;

        // Increment generation counter
        m_t++;

        // Check if we are in the last generation
        if (m_t == m_epsilons.size())
        {
            // Print accepted parameters
            write_parameters(OutputStreamHandler::instance()->getOutputStream(),
                    m_parameter_names, m_prmtr_accepted_new);

            // Terminate Master
            m_p_master->terminate();
            m_entered = false;
            return;
        }

        // Swap population and weights
        std::swap(m_weights_old, m_weights_new);
        std::swap(m_prmtr_accepted_old, m_prmtr_accepted_new);

        // Normalize and compute cumulative sum
        m_weights_cumsum.resize(m_weights_old.size());
        normalize(m_weights_old);
        cumsum(m_weights_old, m_weights_cumsum);

        // Clear m_weights_new, m_prmtr_accepted_new and m_prior_pdf_accepted
        m_weights_new.clear();
        m_prmtr_accepted_new.clear();
        m_prior_pdf_accepted.clear();

        // Flush Master
        m_p_master->flush();
        m_entered = false;

        // Clear m_prior_pdf_pending
        while (!m_prior_pdf_pending.empty())
            m_prior_pdf_pending.pop();

        // Print message
        spdlog::info("Computing generation {}, epsilon = {}", m_t,
                m_epsilons[m_t].str());
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(
                format_simulator_input(
                    m_epsilons[m_t].str(), sampleParameter()));

    m_entered = false;
}

Command ABCSMCController::getSimulator() const
{
    return m_simulator;
}

Parameter ABCSMCController::sampleParameter()
{
    // If in generation 0
    if (m_t == 0)
    {
        // Push dummy prior_pdf to m_prior_pdf_pending
        m_prior_pdf_pending.push(0.0);

        // Sample from prior
        return sample_from_prior(m_prior_sampler);
    }

    // Else, sample from previous population and perturb until the prior pdf is
    // nonzero
    Parameter sampled_parameter;
    double sampled_prior_pdf = 0.0;
    do
    {
        // Sample parameter population
        int idx = sample_population(m_weights_cumsum, m_distribution,
                m_generator);
        Parameter source_parameter = m_prmtr_accepted_old[idx];

        // Perturb parameter
        sampled_parameter = perturb_parameter(m_perturber, m_t,
                source_parameter);

        // Calculate prior_pdf
        sampled_prior_pdf = get_prior_pdf(m_prior_pdf, sampled_parameter);
    } while (sampled_prior_pdf == 0.0);

    // Push sampled_prior_pdf to m_prior_pdf_pending
    m_prior_pdf_pending.push(sampled_prior_pdf);

    return sampled_parameter;
}
