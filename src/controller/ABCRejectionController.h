#ifndef ABCREJECTIONCONTROLLER_H
#define ABCREJECTIONCONTROLLER_H

#include <string>
#include <vector>
#include <istream>

#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Command.h"
#include "interface/types.h"

#include "Sampler.h"

#include "AbstractController.h"

class ABCRejectionController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        // Constructor
        ABCRejectionController(const Input& input_obj);

        // Default destructor
        virtual ~ABCRejectionController() override = default;

        // Iterate function
        virtual void iterate() override;

        // Simulator getter
        virtual Command getSimulator() const override;

        // Static help function
        static std::string help();

        // Static addLongOptions function
        static void addLongOptions(LongOptions& lopts);

        // Static function to make from positional arguments
        static ABCRejectionController* makeController(const Arguments& args);

        // Input struct to contain input to ABCRejectionController
        struct Input
        {
            // Static function to make Input from optional arguments
            static Input makeInput(const Arguments& args);

            // Data
            int number_accept;
            Epsilon epsilon;
            Command simulator;
            std::vector<ParameterName> parameter_names;
            Command prior_sampler;
        };

    private:

        /**** Member variables ****/
        // Epsilon
        Epsilon m_epsilon;

        // Prior sampler
        PriorSampler m_prior_sampler;

        // Parameter names
        std::vector<ParameterName> m_parameter_names;

        // Number of parameter to accept
        int m_number_accept;

        // Vector of accepted parameters
        std::vector<Parameter> m_prmtr_accepted;

        // Simulator command
        Command m_simulator;
};

#endif // ABCREJECTIONCONTROLLER_H
