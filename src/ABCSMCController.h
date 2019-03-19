#ifndef ABCSMCCONTROLLER_H
#define ABCSMCCONTROLLER_H

#include <random>
#include <vector>
#include <string>
#include <memory>

#include "types.h"
#include "LongOptions.h"
#include "Arguments.h"
#include "Sampler.h"
#include "Parameter.h"

#include "AbstractController.h"

class ABCSMCController : public AbstractController
{
    public:

        // Constructor
        ABCSMCController(const smc::input_t &input_obj,
                std::shared_ptr<std::default_random_engine> p_generator,
                int pop_size);

        // Default destructor
        virtual ~ABCSMCController() override = default;

        // Iterate function
        virtual void iterate() override;

        // Simulator getter
        virtual cmd_t getSimulator() const override;

        // Static help function
        static std::string help();

        // Static addLongOptions function
        static void addLongOptions(LongOptions& lopts);

        // Static function to make from positional arguments
        static ABCSMCController* makeController(const Arguments& args);

    private:

        /**** Member variables ****/
        // Epsilons
        std::vector<std::string> m_epsilons;

        // Iteration counter
        int m_t = 0;

        // SMC sampler
        SMCSampler m_smc_sampler;

        // Perturbation pdf for weights calculation
        std::string m_perturbation_pdf;

        // Parameter names
        std::vector<std::string> m_parameter_names;

        // Population size
        int m_pop_size;

        // New accepted parameters
        std::vector<Parameter> m_prmtr_accepted_new;

        // New weights
        std::vector<double> m_weights_new;

        // Simulator command
        cmd_t m_simulator;
};

#endif // ABCSMCCONTROLLER_H
