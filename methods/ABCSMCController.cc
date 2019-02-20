#include <random>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

#include "common.h"
#include "types.h"
#include "run_simulation.h"
#include "write_parameters.h"
#include "smc_weight.h"

#include "ABCSMCController.h"

// Constructor
ABCSMCController::ABCSMCController(const smc::input_t &input_obj,
        std::shared_ptr<std::default_random_engine> p_generator,
        int pop_size) :
    m_epsilons(input_obj.epsilons),
    m_parameter_names(input_obj.parameter_names),
    m_smc_sampler(std::vector<double>(pop_size),
            std::vector<parameter_t>(pop_size), p_generator, input_obj.perturber,
            input_obj.prior_sampler, input_obj.prior_pdf),
    m_perturbation_pdf(input_obj.perturbation_pdf),
    m_pop_size(pop_size),
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

    // If m_t is equal to the number of epsilons, something went wrong because
    // the algorithm should have finished
    assert(m_t < m_epsilons.size());

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
        if (simulation_result(task.getOutputString()) == ACCEPT)
        {
            // Declare parameter
            parameter_t accepted_prmtr;

            // Get input string
            std::stringstream input_sstrm(task.getInputString());

            // Discard epsilon
            std::getline(input_sstrm, accepted_prmtr);

            // Read accepted parameter
            std::getline(input_sstrm, accepted_prmtr);

            // Push accepted parameter
            m_prmtr_accepted_new.push_back(std::move(accepted_prmtr));
        }

#ifndef NDEBUG
        std::cerr << "task.getInputString(): " << task.getInputString() <<
            std::endl;
#endif

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
    if (m_prmtr_accepted_new.size() >= m_pop_size)
    {
        // Trim any superfluous parameters
        while (m_prmtr_accepted_new.size() > m_pop_size)
        {
            m_prmtr_accepted_new.pop_back();
            m_weights_new.pop_back();
        }

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
        return;
    }

    // There is still work to be done, so make sure there are as many tasks
    // queued as there are Managers
    while (m_p_master->needMorePendingTasks())
        m_p_master->pushPendingTask(simulator_input(m_epsilons[m_t],
                    m_smc_sampler.sampleParameter()));

    entered = false;
}

cmd_t ABCSMCController::getSimulator() const
{
    return m_simulator;
}
