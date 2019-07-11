#ifndef ABCSMCCONTROLLER_H
#define ABCSMCCONTROLLER_H

#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <random>

#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Command.h"
#include "interface/types.h"

#include "AbstractController.h"

class ABCSMCController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        // Constructor
        ABCSMCController(const Input &input_obj,
                std::shared_ptr<std::default_random_engine> p_generator);

        // Default destructor
        virtual ~ABCSMCController() override = default;

        // Iterate function
        virtual void iterate() override;

        // Simulator getter
        virtual Command getSimulator() const override;

        // Static help function
        static std::string help();

        // Static addLongOptions function
        static void addLongOptions(LongOptions& lopts);

        // Static function to make from positional arguments
        static ABCSMCController* makeController(const Arguments& args);

        // Input struct to contain input to ABCSMCController
        struct Input
        {
            // Static function to make Input from optional arguments
            static Input makeInput(const Arguments& args);

            // Data
            int population_size;
            std::vector<Epsilon> epsilons;
            Command simulator;
            std::vector<ParameterName> parameter_names;
            Command prior_sampler;
            Command perturber;
            Command prior_pdf;
            Command perturbation_pdf;
        };

    private:

        /**** Member functions ****/
        // Sample parameter
        Parameter sampleParameter();

        /**** Member variables ****/
        // Epsilons
        std::vector<Epsilon> m_epsilons;

        // Iteration counter
        int m_t = 0;

        // Perturbation pdf for weights calculation
        Command m_perturbation_pdf;

        // Parameter names
        std::vector<ParameterName> m_parameter_names;

        // Population size
        int m_population_size;

        // New accepted parameters
        std::vector<Parameter> m_prmtr_accepted_new;

        // New weights
        std::vector<double> m_weights_new;

        // Number of parameters simulated
        int m_number_simulated = 0;

        // Simulator command
        Command m_simulator;

        // Prior pdf values for accepted parameters
        std::vector<double> m_prior_pdf_accepted;

        // Uniform distribution for sampling from population
        std::uniform_real_distribution<double> m_distribution;

        // Random number generator
        std::shared_ptr<std::default_random_engine> m_p_generator;

        // Prior pdf values of pending parameters
        std::queue<double> m_prior_pdf_pending;

        // Parameters accepted in previous generation
        std::vector<Parameter> m_prmtr_accepted_old;

        // Weights of parameters accepted in previous generation
        std::vector<double> m_weights_old;

        // Cumulative sum of weights
        std::vector<double> m_weights_cumsum;

        // Prior sampler command
        Command m_prior_sampler;

        // Perturber command
        Command m_perturber;

        // Prior_pdf command
        Command m_prior_pdf;
};

#endif // ABCSMCCONTROLLER_H
