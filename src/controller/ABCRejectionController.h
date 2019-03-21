#ifndef ABCREJECTIONCONTROLLER_H
#define ABCREJECTIONCONTROLLER_H

#include <string>
#include <vector>
#include <istream>

#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Parameter.h"

#include "Sampler.h"

#include "AbstractController.h"

class ABCRejectionController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        // Constructor
        ABCRejectionController(const Input& input_obj,
                int num_accept);

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
            // Construct from input stream
            Input(std::istream& istrm);

            // Number of lines
            static const int num_lines = 4;

            // Data
            std::string epsilon;
            Command simulator;
            std::vector<std::string> parameter_names;
            Command prior_sampler;
        };

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
        std::vector<Parameter> m_prmtr_accepted;

        // Simulator command
        Command m_simulator;
};

#endif // ABCREJECTIONCONTROLLER_H
