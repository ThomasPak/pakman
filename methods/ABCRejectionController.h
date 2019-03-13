#ifndef ABCREJECTIONCONTROLLER_H
#define ABCREJECTIONCONTROLLER_H

#include <vector>
#include <string>

#include "types.h"
#include "LongOptions.h"
#include "Arguments.h"
#include "Sampler.h"

#include "AbstractController.h"

class ABCRejectionController : public AbstractController
{
    public:

        // Constructor
        ABCRejectionController(const rejection::input_t& input_obj,
                int num_accept);

        // Default destructor
        virtual ~ABCRejectionController() override = default;

        // Iterate function
        virtual void iterate() override;

        // Simulator getter
        virtual cmd_t getSimulator() const override;

        // Static help function
        static std::string help();

        // Static addLongOptions function
        static void addLongOptions(LongOptions& lopts);

        // Static function to make from positional arguments
        static ABCRejectionController* makeController(const Arguments& args);

    private:

        /**** Member variables ****/
        // Epsilon
        std::string m_epsilon;

        // Prior sampler
        PriorSampler m_prior_sampler;

        // Parameter names
        std::vector<std::string> m_parameter_names;

        // Number of parameter to accept
        int m_num_accept;

        // Vector of accepted parameters
        std::vector<parameter_t> m_prmtr_accepted;

        // Simulator command
        cmd_t m_simulator;
};

#endif // ABCREJECTIONCONTROLLER_H
