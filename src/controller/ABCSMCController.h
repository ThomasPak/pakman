#ifndef ABCSMCCONTROLLER_H
#define ABCSMCCONTROLLER_H

#include <random>
#include <vector>
#include <string>
#include <memory>

#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Parameter.h"
#include "core/Command.h"

#include "Sampler.h"

#include "AbstractController.h"

class ABCSMCController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        // Constructor
        ABCSMCController(const Input &input_obj,
                std::shared_ptr<std::default_random_engine> p_generator,
                int pop_size);

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
            // Construct from input stream
            Input(std::istream& istrm);

            // Number of lines
            static const int num_lines = 7;

            // Data
            std::vector<std::string> epsilons;
            Command simulator;
            std::vector<std::string> parameter_names;
            Command prior_sampler;
            Command perturber;
            Command prior_pdf;
            Command perturbation_pdf;
        };

    private:

        /**** Member variables ****/
        // Epsilons
        std::vector<std::string> m_epsilons;

        // Iteration counter
        int m_t = 0;

        // SMC sampler
        SMCSampler m_smc_sampler;

        // Perturbation pdf for weights calculation
        Command m_perturbation_pdf;

        // Parameter names
        std::vector<std::string> m_parameter_names;

        // Population size
        int m_pop_size;

        // New accepted parameters
        std::vector<Parameter> m_prmtr_accepted_new;

        // New weights
        std::vector<double> m_weights_new;

        // Simulator command
        Command m_simulator;
};

#endif // ABCSMCCONTROLLER_H
