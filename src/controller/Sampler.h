#ifndef SAMPLER_H
#define SAMPLER_H

#include <memory>
#include <vector>
#include <string>
#include <queue>
#include <random>
#include <stdexcept>

#include "core/Command.h"
#include "core/Parameter.h"

class AbstractSampler
{

    public:

        virtual Parameter sampleParameter() const = 0;

};

class PriorSampler : public virtual AbstractSampler
{

    public:

        PriorSampler(const Command &prior_sampler);

        virtual Parameter sampleParameter() const override;

    private:

        const Command m_prior_sampler;
};

class PopulationSampler : public virtual AbstractSampler
{

    public:

        PopulationSampler(std::vector<double> weights,
                          std::vector<Parameter> prmtr_population,
                          std::shared_ptr<std::default_random_engine> p_generator);

        void swap_population(std::vector<double> &new_weights,
                             std::vector<Parameter> &new_prmtr_population);

        virtual Parameter sampleParameter() const override;

        const std::vector<double>& getWeights() const
        {
            return m_weights;
        }

        const std::vector<Parameter>& getParameterPopulation() const
        {
            return m_prmtr_population;
        }

    private:

        // Population of parameters to sample from
        std::vector<Parameter> m_prmtr_population;

        // Corresponding weights and cumulative sum
        std::vector<double> m_weights;
        std::vector<double> m_weights_cumsum;

        // Random number generator
        std::shared_ptr<std::default_random_engine> m_p_generator;

        // Uniform distribution
        mutable std::uniform_real_distribution<double> m_distribution;
};

class PerturbationSampler : public virtual AbstractSampler
{

    public:

        PerturbationSampler(const Command &perturber);

        void setT(const int t)
        {
            m_t = t;
        }

        void setBaseParameter(Parameter base_parameter)
        {
            m_base_parameter = std::move(base_parameter);
        }

        Parameter perturbParameter(int t, Parameter prmtr_base) const;

        virtual Parameter sampleParameter() const override;

        int getT() const
        {
            if (m_t == M_INVALID)
            {
                std::runtime_error e("t was not set");
                throw e;
            }
            return m_t;
        }

    private:

        // Perturber command
        const Command m_perturber;

        // Base parameter and t
        int m_t = -1;
        Parameter m_base_parameter;
        constexpr static int M_INVALID = -1;
};

class SMCSampler :
    public PopulationSampler, public PerturbationSampler, public PriorSampler
{

    public:

        SMCSampler(std::vector<double> weights,
                                      std::vector<Parameter> prmtr_population,
                                      std::shared_ptr<std::default_random_engine> p_generator,
                                      const Command &perturber,
                                      const Command &prior_sampler,
                                      const Command &prior_pdf);

        virtual Parameter sampleParameter() const override;

        double getPriorPdf() const
        {
            if (m_prior_pdf_val == M_INVALID)
            {
                std::runtime_error e("prior pdf not available");
                throw e;
            }
            return m_prior_pdf_val;
        }

    private:

        double computePriorPdf(const Parameter& prmtr) const;

        const Command m_prior_pdf;
        mutable double m_prior_pdf_val;

        constexpr static double M_INVALID = -1.0;
};

class Generator : public virtual AbstractSampler
{

    public:

        Generator(const Command &generator);

        virtual Parameter sampleParameter() const override;

        int getNumberOfParameters() const
        {
            return m_num_param;
        }

    private:

        mutable std::queue<Parameter> m_param_queue;

        int m_num_param;
};

#endif // SAMPLER_H
