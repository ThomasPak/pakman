#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <utility>
#include <stdexcept>

#include "types.h"
#include "interface/protocols.h"
#include "system_call.h"
#include "vector_strtok.h"
#include "sample_population.h"
#include "Parameter.h"
#include "Sampler.h"

/**** PriorSampler ****/
PriorSampler::PriorSampler(const cmd_t &prior_sampler) : m_prior_sampler(prior_sampler) {}

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
PerturbationSampler::PerturbationSampler(const cmd_t &perturber) : m_perturber(perturber) {}

Parameter PerturbationSampler::perturbParameter(int t, Parameter prmtr_base) const
{

    // Prepare input to perturber
    std::string input;
    input += std::to_string(t);
    input += '\n';
    input += prmtr_base.str();
    input += '\n';

    // Call perturber
    std::string raw_prmtr_sample;
    system_call(m_perturber, input, raw_prmtr_sample);

    return raw_prmtr_sample;
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
                              const cmd_t &perturber,
                              const cmd_t &prior_sampler,
                              const cmd_t &prior_pdf) :
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
    std::string input(prmtr.str());
    input += "\n";
    std::string prmtr_prior_pdf_str;
    system_call(m_prior_pdf, input, prmtr_prior_pdf_str);
    return std::stod(prmtr_prior_pdf_str);
}

/**** Generator ****/
Generator::Generator(const cmd_t &generator)
{

    // Read output from generator
    std::string generator_output;
    system_call(generator, generator_output);

    // Decompose into individual parameters
    std::vector<std::string> parameter_list;
    vector_strtok(generator_output, parameter_list, "\n");

    // Save number of parameters
    m_num_param = parameter_list.size();

    // Sanity check: at least one parameter should have been generated
    if (m_num_param == 0)
    {
        std::runtime_error e("generator did not output any parameters");
        throw e;
    }

    // Push onto queue
    for (const auto & prmtr : parameter_list)
        m_param_queue.push(prmtr);
}

Parameter Generator::sampleParameter() const
{

    // Get parameter from front of queue
    Parameter prmtr = m_param_queue.front();

    // Pop if not last element
    if (m_param_queue.size() > 1)
        m_param_queue.pop();

    return prmtr;
}
