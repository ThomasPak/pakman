#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>
#include <string>
#include <queue>
#include <random>
#include <stdexcept>

#include "types.h"

class AbstractSampler {

    public:

        virtual parameter_t sampleParameter() const = 0;

    protected:

        parameter_t removeTrailingWhitespace(const std::string& sampler_output) const;
};

class PriorSampler : public virtual AbstractSampler {

    public:

        PriorSampler(const cmd_t &prior_sampler);

        virtual parameter_t sampleParameter() const override;

    private:

        const cmd_t m_prior_sampler;
};

class PopulationSampler : public virtual AbstractSampler {

    public:

        PopulationSampler(std::vector<double> weights,
                          std::vector<parameter_t> prmtr_population,
                          std::default_random_engine &generator);

        void swap_population(std::vector<double> &new_weights,
                             std::vector<parameter_t> &new_prmtr_population);

        virtual parameter_t sampleParameter() const override;

        const std::vector<double>& getWeights() const {
            return m_weights;
        }

        const std::vector<parameter_t>& getParameterPopulation() const {
            return m_prmtr_population;
        }

    private:

        // Population of parameters to sample from
        std::vector<parameter_t> m_prmtr_population;

        // Corresponding weights and cumulative sum
        std::vector<double> m_weights;
        std::vector<double> m_weights_cumsum;

        // Random number generator
        std::default_random_engine &m_generator;

        // Uniform distribution
        mutable std::uniform_real_distribution<double> m_distribution;
};

class PerturbationSampler : public virtual AbstractSampler {

    public:

        PerturbationSampler(const cmd_t &perturber);

        void setT(const int t) {
            m_t = t;
        }

        void setBaseParameter(parameter_t base_parameter) {
            m_base_parameter.assign(base_parameter);
        }

        parameter_t perturbParameter(int t, parameter_t prmtr_base) const;

        virtual parameter_t sampleParameter() const override;

        int getT() const {
            if (m_t == M_INVALID) {
                std::runtime_error e("t was not set");
                throw e;
            }
            return m_t;
        }

    private:

        // Perturber command
        const cmd_t m_perturber;

        // Base parameter and t
        int m_t = -1;
        parameter_t m_base_parameter;
        constexpr static int M_INVALID = -1;
};

class SMCSampler :
    public PopulationSampler, public PerturbationSampler, public PriorSampler {

    public:

        SMCSampler(std::vector<double> weights,
                                      std::vector<parameter_t> prmtr_population,
                                      std::default_random_engine &generator, const cmd_t &perturber,
                                      const cmd_t &prior_sampler,
                                      const cmd_t &prior_pdf);

        virtual parameter_t sampleParameter() const override;

        double getPriorPdf() const {
            if (m_prior_pdf_val == M_INVALID) {
                std::runtime_error e("prior pdf not available");
                throw e;
            }
            return m_prior_pdf_val;
        }

    private:

        double computePriorPdf(const parameter_t& prmtr) const;

        const cmd_t m_prior_pdf;
        mutable double m_prior_pdf_val;

        constexpr static double M_INVALID = -1.0;
};

class Generator : public virtual AbstractSampler {

    public:

        Generator(const cmd_t &generator);

        virtual parameter_t sampleParameter() const override;

        int getNumberOfParameters() const {
            return m_num_param;
        }

    private:

        mutable std::queue<parameter_t> m_param_queue;

        int m_num_param;
};

#endif // SAMPLER_H
