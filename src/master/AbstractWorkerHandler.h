#ifndef ABSTRACTWORKERHANDLER_H
#define ABSTRACTWORKERHANDLER_H

#include <string>

#include "core/Command.h"

/** An abstract class for representing Workers.
 *
 * Workers are instantiations of the simulator user executable.  Since they can
 * be implemented as forked processes or MPI processes, this abstract class was
 * made to provide an interface to both types.
 *
 * When an AbstractWorkerHandler is constructed, the simulation is initiated
 * immediately.  Hence, there are no member functions to start the simulation,
 * only functions to check whether the simulation has finished (isDone()) and
 * to obtain the results (getOutput() and getErrorCode()).
 *
 * The destructor of a WorkerHandler ensures that the simulation is terminated,
 * but the Worker process may persist, as in the case of MPIWorkerHandler.
 */

class AbstractWorkerHandler
{

    public:

        /** Construct from simulator string and input string.
         *
         * @param simulator  command to run simulation.
         * @param input_string  input string to simulator.
         */
        AbstractWorkerHandler(const Command& simulator,
                const std::string& input_string);

        /** Default destructor does nothing. */
        virtual ~AbstractWorkerHandler() = default;

        /** @return whether Worker has finished. */
        virtual bool isDone() = 0;

        /** @return output of finished Worker.
         *
         * @warning Calling this function before Worker is finished will result
         * in an error, so always check with isDone() first.
         */
        std::string getOutput();

        /** @return error code of finished Worker.
         *
         * @warning Calling this function before Worker is finished will result
         * in an error, so always check with isDone() first.
         */
        int getErrorCode();

    protected:

        /** Command to run simulation. */
        const Command m_simulator;

        /** Input string to simulator. */
        const std::string m_input_string;

        /** Buffer to contain simulator output. */
        std::string m_output_buffer;

        /** Error code received from simulator. */
        int m_error_code = -1;
};

#endif // ABSTRACTWORKERHANDLER_H
