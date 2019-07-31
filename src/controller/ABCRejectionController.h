#ifndef ABCREJECTIONCONTROLLER_H
#define ABCREJECTIONCONTROLLER_H

#include <string>
#include <vector>
#include <istream>

#include "core/Command.h"

#include "AbstractController.h"

class LongOptions;
class Arguments;

/** A Controller class implementing the ABC rejection algorithm.
 *
 * The ABCRejectionController class implements the ABC rejection algorithm,
 * which consists of the following steps:
 *
 * 1. Sample a candidate parameter \f$\theta^* \sim P(\theta)\f$, where
 * \f$P(\theta)\f$ is the prior distribution.
 *
 * 2. Run simulation with \f$\theta^*\f$ to generate simulated data \f$D^* \sim
 * P(D \mid \theta^*)\f$, where \f$P(D \mid \theta^*)\f$ is the likelihood
 * function.
 *
 * 3. If \f$d(D^*, D_0) \leq \epsilon\f$, accept \f$\theta^*\f$, else reject.
 * Here, \f$d(\cdot, \cdot)\f$ is the distance function, \f$D_0\f$ is the
 * observed data, and \f$\epsilon\f$ is the tolerance.
 *
 * Steps 1--3 are repeated until the desired number of accepted parameters is
 * reached.
 *
 * For instructions on how to use Pakman with the ABC rejection controller,
 * execute the following command
 * ```
 * $ pakman rejection --help
 * ```
 */

class ABCRejectionController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        /** Construct from Input object.
         *
         * @param input_obj  Input object.
         */
        ABCRejectionController(const Input& input_obj);

        /** Default destructor does nothing. */
        virtual ~ABCRejectionController() override = default;

        /** Iterates the ABCRejectionController.  Should be called by a Master.
         */
        virtual void iterate() override;

        /** @return simulator command. */
        virtual Command getSimulator() const override;

        /** @return help message string. */
        static std::string help();

        /** Add long command-line options.
         *
         * @param lopts  long command-line options that the
         * ABCRejectionController needs.
         */
        static void addLongOptions(LongOptions& lopts);

        /** Create ABCRejectionController instance.
         *
         * @param args  command-line arguments.
         *
         * @return pointer to created ABCRejectionController instance.
         */
        static ABCRejectionController* makeController(const Arguments& args);

        /** Input struct thats contains input to ABCRejectionController
         * constructor. */
        struct Input
        {
            /** Static function to make Input from command-line arguments.
             *
             * @param args  command-line arguments.
             *
             * @return Input struct made from command-line arguments.
             */
            static Input makeInput(const Arguments& args);

            /** Number of parameters to accept. */
            int number_accept;

            /** Distance threshold for acceptance. */
            Epsilon epsilon;

            /** Command to run simulation. */
            Command simulator;

            /** List of parameter names. */
            std::vector<ParameterName> parameter_names;

            /** Command to run sample from prior. */
            Command prior_sampler;
        };

    private:

        ///// Member variables /////
        // Epsilon
        Epsilon m_epsilon;

        // Parameter names
        std::vector<ParameterName> m_parameter_names;

        // Number of parameter to accept
        int m_number_accept;

        // Vector of accepted parameters
        std::vector<Parameter> m_prmtr_accepted;

        // Number of parameters simulated
        int m_number_simulated = 0;

        // Simulator command
        Command m_simulator;

        // Prior_sampler command
        Command m_prior_sampler;

        // Entered iterate()
        bool m_entered = false;
};

#endif // ABCREJECTIONCONTROLLER_H
