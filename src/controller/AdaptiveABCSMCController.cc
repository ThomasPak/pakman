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

#include "AdaptiveABCSMCController.h"

// Constructor
AdaptiveABCSMCController::AdaptiveABCSMCController(const Input &input_obj) :
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
void AdaptiveABCSMCController::iterate()
{
    // This function should never be called recursively
    assert(!m_entered);
    m_entered = true;

    // [INSERT CODE]

    m_entered = false;
}

Command AdaptiveABCSMCController::getSimulator() const
{
    return m_simulator;
}

Parameter AdaptiveABCSMCController::sampleParameter()
{
    // [INSERT CODE]
}
