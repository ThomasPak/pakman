#ifndef SERIALMASTER_H
#define SERIALMASTER_H

#include <string>
#include <queue>

#include "core/common.h"

#include "AbstractMaster.h"

class LongOptions;
class Arguments;

/** A Master class for performing simulation tasks serially.
 *
 * The SerialMaster class performs simulation tasks serially by spawning child
 * processes with `fork()`--`exec()` to run simulations.
 *
 * For instructions on how to use Pakman with the serial master, execute the
 * following command
 * ```
 * $ pakman serial --help
 * ```
 */

class SerialMaster : public AbstractMaster
{
    public:

        /** Constructor saves simulator command and program termination flag.
         *
         * @param simulator  command to run simulation.
         * @param p_program_terminated  pointer to boolean flag that is set
         * when the execution of Pakman is terminated by the user.
         */
        SerialMaster(const Command& simulator, bool *p_program_terminated);

        /** Default destructor does nothing. */
        virtual ~SerialMaster() override = default;

        /** @return whether the AbstractMaster is active. */
        virtual bool isActive() const override;

        /** @return whether more pending tasks are needed. */
        virtual bool needMorePendingTasks() const override;

        /** Push a new pending task.
         *
         * @param input_string  input string to simulation job.
         */
        virtual void pushPendingTask(const std::string& input_string) override;

        /** @return whether finished tasks queue is empty. */
        virtual bool finishedTasksEmpty() const override;

        /** @return reference to front finished task. */
        virtual TaskHandler& frontFinishedTask() override;

        /** Pop front finished task. */
        virtual void popFinishedTask() override;

        /** Flush all finished, busy and pending tasks. */
        virtual void flush() override;

        /** Terminate SerialMaster. */
        virtual void terminate() override;

        /** @return help message string. */
        static std::string help();

        /** Add long command-line options.
         *
         * @param lopts  long command-line options that the SerialMaster needs.
         */
        static void addLongOptions(LongOptions& lopts);

        /** Run SerialMaster in an event loop.
         *
         * This function creates the SerialMaster and Controller objects, and
         * runs them in an event loop.
         *
         * @param controller  controller type.
         * @param args  command-line arguments.
         */
        static void run(controller_t controller, const Arguments& args);

        /** For SerialMaster, this function does nothing. */
        static void cleanup();

    protected:

        /** Iterates the SerialMaster in an event loop. */
        virtual void iterate() override;

    private:

        /** Enumerate type for SerialMaster states.
         *
         * The SerialMaster can either in a `normal` state, or in a
         * `terminated` state.  When the SerialMaster is in a `terminated`
         * state, the member function isActive() will return false and the
         * event loop should terminate.
         */
        enum state_t { normal, terminated };

        ///// Member functions /////
        // Processes a task from pending queue if there is one and places it in
        // the finished queue when done.
        void processTask();

        ///// Member variables /////
        // Initial state is normal
        state_t m_state = normal;

        // Simulator command
        const Command m_simulator;

        // Finished tasks
        std::queue<TaskHandler> m_finished_tasks;

        // Pending tasks
        std::queue<TaskHandler> m_pending_tasks;

        // Entered iterate()
        bool m_entered = false;
};

#endif // SERIALMASTER_H
