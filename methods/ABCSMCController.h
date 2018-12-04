#ifndef ABCSMCCONTROLLER_H
#define ABCSMCCONTROLLER_H

#include <random>
#include <vector>
#include <string>

#include "Sampler.h"

#include "AbstractController.h"

class ABCSMCController : public AbstractController
{
    public:

        // Constructor
        ABCSMCController(const smc::input_t &input_obj,
                std::default_random_engine &generator, int pop_size);

        // Default destructor
        virtual ~ABCSMCController() override = default;

        // Iterate function
        virtual void iterate() override;

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
        std::vector<parameter_t> m_prmtr_accepted_new;

        // New weights
        std::vector<double> m_weights_new;
};

#endif // ABCSMCCONTROLLER_H
