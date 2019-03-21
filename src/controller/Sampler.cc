#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <utility>
#include <stdexcept>

#include "core/types.h"
#include "core/utils.h"
#include "core/Parameter.h"
#include "system/system_call.h"
#include "interface/protocols.h"

#include "sample_population.h"

#include "Sampler.h"

/**** PriorSampler ****/
PriorSampler::PriorSampler(const Command &prior_sampler) : m_prior_sampler(prior_sampler) {}

Parameter PriorSampler::sampleParameter() const
{

    std::string prior_sampler_output;
    system_call(m_prior_sampler, prior_sampler_output);

    return parse_prior_sampler_output(prior_sampler_output);
}

/**** PopulationSampler ****/
PopulationSampler::PopulationSampler(std::vector<double> weights,
                  std::vector<Parameter> prmtr_population,
                  std::shared_ptr<std::default_random_engine> p_generator) :
                  m_weights(weights),
                  m_weights_cumsum(weights.size()),
                  m_prmtr_population(prmtr_population),
                  m_p_generator(p_generator),
                  m_distribution(0.0, 1.0)
{

    // Normalize and compute cumulative sum
    normalize(m_weights);
    cumsum(m_weights, m_weights_cumsum);
}

void PopulationSampler::swap_population(std::vector<double> &new_weights,
                     std::vector<Parameter> &new_prmtr_population)
{

    // Swap weights and parameter population
    std::swap(m_weights, new_weights);
    std::swap(m_prmtr_population, new_prmtr_population);

    // Normalize and compute cumulative sum
    m_weights_cumsum.resize(m_weights.size());
    normalize(m_weights);
    cumsum(m_weights, m_weights_cumsum);
}

Parameter PopulationSampler::sampleParameter() const
{

    // Sample population
    int idx = sample_population(m_weights_cumsum, m_distribution, *m_p_generator);
    return m_prmtr_population[idx];
}


/**** PerturbationSampler ****/
PerturbationSampler::PerturbationSampler(const Command &perturber) : m_perturber(perturber) {}

Parameter PerturbationSampler::perturbParameter(int t, Parameter prmtr_base) const
{

    // Prepare input to perturber
    std::string perturber_input = format_perturber_input(t, prmtr_base);

    // Call perturber
    std::string perturber_output;
    system_call(m_perturber, perturber_input, perturber_output);

    return parse_perturber_output(perturber_output);
}

Parameter PerturbationSampler::sampleParameter() const
{

    // Check that t has been assigned
    if (m_t == -1)
    {
        std::runtime_error e("need to set t before perturbing");
        throw e;
    }

    // Check that base parameter has been assigned
    if (m_base_parameter.size() == 0)
    {
        std::runtime_error e("need to set base parameter before perturbing");
        throw e;
    }

    return perturbParameter(m_t, m_base_parameter);
}

/**** SMCSampler ****/
SMCSampler::SMCSampler(std::vector<double> weights,
                              std::vector<Parameter> prmtr_population,
                              std::shared_ptr<std::default_random_engine> p_generator,
                              const Command &perturber,
                              const Command &prior_sampler,
                              const Command &prior_pdf) :
    PopulationSampler(weights, prmtr_population, p_generator),
    PerturbationSampler(perturber),
    PriorSampler(prior_sampler),
    m_prior_pdf(prior_pdf),
    m_prior_pdf_val(M_INVALID) {}

Parameter SMCSampler::sampleParameter() const
{

    // Flag prior_pdf to indicate the value is invalid
    m_prior_pdf_val = M_INVALID;

    // Initialize temporary variables
    Parameter prmtr_sampled;
    double temp_prior_pdf;

    // If in generation 0, sample from prior
    if (getT() == 0)
    {
        prmtr_sampled = this->PriorSampler::sampleParameter();

        m_prior_pdf_val = computePriorPdf(prmtr_sampled);
        return prmtr_sampled;
    }

    do
    {
        // Sample parameter population
        Parameter prmtr_base = this->PopulationSampler::sampleParameter();

        // Perturb parameter
        prmtr_sampled = perturbParameter(getT(), std::move(prmtr_base));
    } while ((temp_prior_pdf = computePriorPdf(prmtr_sampled)) == 0.0);

    // Store value
    m_prior_pdf_val = temp_prior_pdf;

    // Return parameter
    return prmtr_sampled;
}

double SMCSampler::computePriorPdf(const Parameter& prmtr) const
{
    // Prepare input to prior_pdf
    std::string prior_pdf_input = format_prior_pdf_input(prmtr);

    // Call prior_pdf
    std::string prior_pdf_output;
    system_call(m_prior_pdf, prior_pdf_input, prior_pdf_output);

    return parse_prior_pdf_output(prior_pdf_output);
}
