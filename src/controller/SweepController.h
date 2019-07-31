#ifndef SWEEPCONTROLLER_H
#define SWEEPCONTROLLER_H

#include <string>
#include <vector>

#include "interface/types.h"

#include "AbstractController.h"

/** A Controller class implementing a simple parameter sweep algorithm.
 *
 * The SweepController class implements a simple parameter sweep algorithm.
 * The parameter sets to simulate are given by the Input::generator command.
 * The simulator is then called for each of these parameter sets, and the
 * output of the simulator is discarded.
 *
 * For instructions on how to use Pakman with the sweep controller, execute the
 * following command
 * ```
 * $ pakman sweep --help
 * ```
 */

class SweepController : public AbstractController
{
    public:

        // Forward declaration of Input
        struct Input;

        /** Construct from Input object.
         *
         * @param input_obj  Input object.
         */
        SweepController(const Input &input_obj);

        /** Default destructor does nothing. */
        virtual ~SweepController() override = default;

        /** Iterates the SweepController.  Should be called by a Master. */
        virtual void iterate() override;

        /** @return simulator command. */
        virtual Command getSimulator() const override;

        /** @return help message string. */
        static std::string help();

        /** Add long command-line options.
         *
         * @param lopts  long command-line options that the SweepController
         * needs.
         */
        static void addLongOptions(LongOptions& lopts);

        /** Create SweepController instance.
         *
         * @param args  command-line arguments.
         *
         * @return pointer to created SweepController instance.
         */
        static SweepController* makeController(const Arguments& args);

        /** Input struct thats contains input to SweepController
         * constructor.
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

            /** Command to run simulation. */
            Command simulator;

            /** List of parameter names. */
            std::vector<ParameterName> parameter_names;

            /** Command to generate parameter sets to simulate. */
            Command generator;
        };

    private:

        ///// Member variables /////
        // Parameter names
        std::vector<ParameterName> m_parameter_names;

        // Parameter list
        std::vector<Parameter> m_prmtr_list;

        // Counter for number of finished parameters
        int m_num_finished = 0;

        // At first iteration, SweepController will push all parameters to the
        // pending queue of the Master.  This flag tells iterate() that it is
        // in the first iteration.
        bool m_first_iteration = true;

        // Simulator command
        Command m_simulator;

        // Entered iterate()
        bool m_entered = false;
};

#endif // SWEEPCONTROLLER_H
