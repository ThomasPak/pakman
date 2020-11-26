#ifndef ABCSMCCONTROLLER_H
#define ABCSMCCONTROLLER_H

#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <random>
#include <chrono>

#include "core/Command.h"

#include "AbstractController.h"

class LongOptions;
class Arguments;

/** A Controller class implementing the ABC SMC algorithm.
 *
 * The ABCSMCController class implements the ABC SMC algorithm, which is
 * detailed in the following paper:
 *
 * > Toni, Tina, David Welch, Natalja Strelkowa, Andreas Ipsen, and Michael
 * P.H.  > Stumpf. 2009.  “Approximate Bayesian computation scheme for
 * parameter > inference and model selection in dynamical systems.” J. R. Soc.
 * Interface 6 > (31): 187–202. doi:10.1098/rsif.2008.0172.
 *
 * For instructions on how to use Pakman with the ABC SMC controller, execute
 * the following command
 * ```
 * $ pakman smc --help
 * ```
 */

class ABCSMCController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        /** Construct from Input object and pointer to random number engine.
         *
         * The random number engine is used to choose a parameter from the
         * parameter population.
         *
         * @param input_obj  Input object.
         * @param p_generator  pointer to random number engine.
         */
        ABCSMCController(const Input &input_obj);

        /** Default destructor does nothing. */
        virtual ~ABCSMCController() override = default;

        /** Iterates the ABCSMCController.  Should be called by a Master.  */
        virtual void iterate() override;

        /** @return simulator command. */
        virtual Command getSimulator() const override;

        /** @return help message string. */
        static std::string help();

        /** Add long command-line options.
         *
         * @param lopts  long command-line options that the ABCSMCController
         * needs.
         */
        static void addLongOptions(LongOptions& lopts);

        /** Create ABCSMCController instance.
         *
         * @param args  command-line arguments.
         *
         * @return pointer to created ABCSMCController instance.
         */
        static ABCSMCController* makeController(const Arguments& args);

        /** Input struct thats contains input to ABCSMCController constructor.
         */
        struct Input
        {
            /** Static function to make Input from command-line arguments.
             *
             * @param args  command-line arguments.
             *
             * @return Input struct made from command-line arguments.
             */
            static Input makeInput(const Arguments& args);

            /** Population size. */
            int population_size;

            /** Distance threshold series. */
            std::vector<Epsilon> epsilons;

            /** Command to run simulation. */
            Command simulator;

            /** List of parameter names. */
            std::vector<ParameterName> parameter_names;

            /** Command to run sample from prior. */
            Command prior_sampler;

            /** Command to perturb parameter. */
            Command perturber;

            /** Command to obtain probability density of prior distribution. */
            Command prior_pdf;

            /** Command to obtain probability density of perturbation kernel
             * distribution.
             */
            Command perturbation_pdf;

            /** Seed for pseudo random number generator */
            unsigned long seed =
                std::chrono::system_clock::now().time_since_epoch().count();
        };

    private:

        ///// Member functions /////
        // Sample parameter
        Parameter sampleParameter();

        ///// Member variables /////
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
        std::mt19937_64 m_generator;

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

        // First iteration
        bool m_first = true;

        // Entered iterate()
        bool m_entered = false;
};

#endif // ABCSMCCONTROLLER_H
