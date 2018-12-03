#ifndef ABCREJECTIONCONTROLLER_H
#define ABCREJECTIONCONTROLLER_H

#include "Sampler.h"

#include "AbstractController.h"

class ABCRejectionController : public AbstractController
{
    public:

        // Constructor
        ABCRejectionController(const rejection::input_t& input_obj,
                int num_accept);

        // Default constructor
        virtual ~ABCRejectionController() = default;

        // Iterate function
        virtual void iterate();

    private:

        /**** Member functions ****/
        void printAcceptedParameters();

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
};

#endif // ABCREJECTIONCONTROLLER_H
