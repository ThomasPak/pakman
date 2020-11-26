#ifndef ABSTRACTMASTER_H
#define ABSTRACTMASTER_H

#include <memory>
#include <string>

#include "core/common.h"
#include "core/TaskHandler.h"

class AbstractController;
class LongOptions;
class Arguments;

/** An abstract class for performing simulation tasks.
 *
 * AbstractMasters are responsible for performing simulation tasks that it
 * receives from an instantiation of the AbstractController class.
 *
 * A simulation task consists of spawning a simulator, feeding it some input
 * and retrieving the output.  Simulation tasks are represented by the
 * TaskHandler class.
 *
 * The AbstractMaster::assignController() and
 * AbstractController::assignMaster() methods are used for assigning
 * AbstractMaster and AbstractController objects to each other.
 *
 * AbstractMasters are designed for use in an event loop.  The
 * AbstractMaster::iterate() method should be called repeatedly until
 * isActive() returns false.  The terminate() method should be called by
 * AbstractController when it is no longer needed.
 *
 * The task processing machinery of AbstractMaster is abstracted using task
 * queues; AbstractControllers push tasks that need to be completed with
 * pushPendingTask() and access finished tasks using frontFinishedTask().  The
 * AbstractMaster is responsible for popping tasks from the pending tasks
 * queue, running the corresponding simulation and pushing finished tasks to
 * the finished tasks queue.  The finished tasks should be pushed in the same
 * order as they were added to the pending tasks queue.  The flush() method
 * flushes all queues and discards all running simulations.
 *
 * The use of AbstractMaster is governed by static methods.  The static
 * addLongOptions() and help() methods determine which command-line options the
 * Master accepts and return a help message explaining the options,
 * respectively.  The static run() method sets up the appropriate Master and
 * runs it in an event loop.
 */

class AbstractMaster
{
    public:

        /** Constructor saves program termination flag.
         *
         * @param p_program_terminated  pointer to boolean flag that is set
         * when the execution of Pakman is terminated by the user.
         */
        AbstractMaster(bool *p_program_terminated);

        /** Default destructor does nothing. */
        virtual ~AbstractMaster() = default;

        /** @return whether the AbstractMaster is active. */
        virtual bool isActive() const = 0;

        /** @return whether more pending tasks are needed. */
        virtual bool needMorePendingTasks() const = 0;

        /** Push a new pending task.
         *
         * @param input_string  input string to simulation job.
         */
        virtual void pushPendingTask(const std::string& input_string) = 0;

        /** @return whether finished tasks queue is empty. */
        virtual bool finishedTasksEmpty() const = 0;

        /** @return reference to front finished task. */
        virtual TaskHandler& frontFinishedTask() = 0;

        /** Pop front finished task. */
        virtual void popFinishedTask() = 0;

        /** Flush all finished, busy and pending tasks. */
        virtual void flush() = 0;

        /** Terminate AbstractMaster. */
        virtual void terminate() = 0;

        /** Interpret string as Master type.
         *
         * The master_t enumeration type is defined in common.h.
         *
         * @param arg  string to be interpreted.
         *
         * @return the Master type.
         */
        static master_t getMaster(const std::string& arg);

        /** Return help message based on Master type.
         *
         * When subclassing AbstractMaster, be sure to include a static method
         * with the signature
         *
         * ```
         * static std::string help()
         * ```
         *
         * and add an entry in the switch statement of AbstractMaster::help().
         *
         * @param master  Master type.
         *
         * @return help message string.
         */
        static std::string help(master_t master);

        /** Add Master-specific long options to the given LongOptions object.
         *
         * When subclassing AbstractMaster, be sure to include a static method
         * with the signature
         *
         * ```
         * static void addLongOptions(LongOptions& lopts)
         * ```
         *
         * and add an entry in the switch statement of
         * AbstractMaster::addLongOptions().
         *
         * @param master  Master type.
         * @param lopts  long options that the Master needs.
         */
        static void addLongOptions(master_t master, LongOptions& lopts);

        /** Execute run function based on Master type.
         *
         * When subclassing AbstractMaster, be sure to include a static method
         * with the signature
         *
         * ```
         * static void run(controller_t controller, const Arguments& args)
         * ```
         *
         * and add an entry in the switch statement of AbstractMaster::run().
         *
         * @param master  Master type.
         * @param controller  Controller type.
         * @param args  command-line arguments.
         */
        static void run(master_t master, controller_t controller,
                const Arguments& args);

        /** Execute cleanup function based on Master type.
         *
         * When subclassing AbstractMaster, be sure to include a static method
         * with the signature
         *
         * ```
         * static void cleanup()
         * ```
         *
         * and add an entry in the switch statement of
         * AbstractMaster::cleanup().
         *
         * @param master  Master type.
         */
        static void cleanup(master_t master);

    protected:

        /** Assign pointer to AbstractController.
         *
         * @param p_controller  pointer to AbstractController object to be
         * assigned to AbstractMaster.
         */
        void assignController(
                std::shared_ptr<AbstractController> p_controller);

        /** Iterates the AbstractMaster in an event loop. */
        virtual void iterate() = 0;

        /** @return whether the program has been terminated. */
        bool programTerminated() const;

        ///// Member variables /////
        /** Weak pointer to AbstractController. */
        std::weak_ptr<AbstractController> m_p_controller;

    private:

        ///// Member variables /////
        // Pointer to program terminated flag
        bool *m_p_program_terminated;
};

#endif // ABSTRACTMASTER_H
