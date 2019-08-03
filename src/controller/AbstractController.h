#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

#include <memory>
#include <vector>
#include <string>

#include "core/common.h"

class AbstractMaster;
class LongOptions;
class Arguments;
class Command;

/** An abstract class for submitting simulation tasks.
 *
 * AbstractControllers are responsible for submitting simulation tasks to an
 * instantiation of AbstractMaster, as part of some algorithm.
 *
 * The AbstractController::assignMaster() and
 * AbstractMaster::assignController() methods are used for assigning
 * AbstractMaster and AbstractController objects to each other.
 *
 * AbstractControllers are designed for use in an event loop.  The
 * AbstractController::iterate() method should be called repeatedly by
 * AbstractMaster.  The AbstractController should call
 * AbstractMaster::terminate() on its assigned AbstractMaster when the
 * algorithm has finished.
 *
 * From the perspective of AbstractController, the AbstractMaster is a black
 * box that it pushes tasks to (via AbstractMaster::pushPendingTask()), and
 * retrieves finished tasks from (via AbstractMaster::frontFinishedTask()).
 * This simplifies the design of Controller classes to implement new
 * algorithms, since the programmer does not need to worry about the execution
 * of the tasks.
 *
 * The use of AbstractController is governed by static methods.  The static
 * addLongOptions() and help() methods determine which command-line options the
 * Controller accepts and return a help message explaining the options,
 * respectively.  The static makeController() method is a factory method that
 * creates a Controller from command-line arguments.
 */

class AbstractController
{
    public:

        /** Default constructor does nothing. */
        AbstractController() = default;

        /** Default destructor does nothing. */
        virtual ~AbstractController() = default;

        /** Assign pointer to AbstractMaster.
         *
         * @param p_master  pointer to AbstractMaster object to be assigned to
         * AbstractController.
         */
        void assignMaster(std::shared_ptr<AbstractMaster> p_master);

        /** Iterates the AbstractController.  Should be called by a Master. */
        virtual void iterate() = 0;

        /** @return simulator command. */
        virtual Command getSimulator() const = 0;

        /** Interpret string as Controller type.
         *
         * The controller_t enumeration type is defined in common.h.
         *
         * @param arg  string to be interpreted.
         *
         * @return the Controller type.
         */
        static controller_t getController(const std::string& arg);

        /** Return help message based on Controller type.
         *
         * When subclassing AbstractController, be sure to include a static
         * method with the signature
         *
         * ```
         * static std::string help()
         * ```
         *
         * and add an entry in the switch statement of
         * AbstractController::help().
         *
         * @param controller  Controller type.
         *
         * @return help message string.
         */
        static std::string help(controller_t controller);

        /** Add Controller-specific long command-line options to the given
         * LongOptions object.
         *
         * When subclassing AbstractController, be sure to include a static
         * method with the signature
         *
         * ```
         * static void addLongOptions(LongOptions& lopts)
         * ```
         *
         * and add an entry in the switch statement of
         * AbstractController::addLongOptions().
         *
         * @param controller  Controller type.
         * @param lopts  long command-line options that the Controller needs.
         */
        static void addLongOptions(controller_t controller,
                LongOptions& lopts);

        /** Create Controller instance based on Controller type.
         *
         * When subclassing AbstractController, be sure to include a static
         * method with the signature
         *
         * ```
         * static AbstractController* makeController(const Arguments& args)
         * ```
         *
         * and add an entry in the switch statement of
         * AbstractController::help()
         *
         * @param controller  Controller type.
         * @param args  command-line arguments.
         *
         * @return pointer to created Controller instance.
         */
        static AbstractController* makeController(controller_t controller,
                const Arguments& args);

    protected:

        /** Shared pointer to AbstractMaster. */
        std::shared_ptr<AbstractMaster> m_p_master;
};

#endif // ABSTRACTCONTROLLER_H
