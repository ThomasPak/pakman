#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

#include <memory>
#include <vector>
#include <string>

#include "core/types.h"
#include "core/common.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"

#include "master/AbstractMaster.h"

class AbstractMaster;

class AbstractController
{
    public:

        // Default constructor
        AbstractController() = default;

        // Virtual default destructor
        virtual ~AbstractController() = default;

        // Assign Master
        void assignMaster(std::shared_ptr<AbstractMaster> p_master);

        // Pure virtual iterate function
        virtual void iterate() = 0;

        // Pure virtual simulator getter
        virtual Command getSimulator() const = 0;

        // Return controller type based on string
        static controller_t getController(const std::string& arg);

        // Return help message based on controller type.
        // When subclassing AbstractController, be sure to include a static
        // method with the signature
        //
        // 'static std::string help()'
        //
        // and add an entry in the switch statement of
        // AbstractController::help()
        static std::string help(controller_t controller);

        // Add Controller-specific long options to the given LongOptions
        // object.
        // When subclassing AbstractController, be sure to include a static
        // method with the signature
        //
        // 'static void addLongOptions(LongOptions& lopts)'
        //
        // and add an entry in
        // the switch statement of AbstractController::addLongOptions()
        static void addLongOptions(controller_t controller,
                LongOptions& lopts);

        // Create Controller instance based on controller type
        // When subclassing AbstractController, be sure to include a static
        // method with the signature
        //
        // 'static AbstractController* makeController(const Arguments& args)'
        //
        // and add an entry in the switch statement of
        // AbstractController::help()
        static AbstractController* makeController(controller_t controller,
                const Arguments& args);

    protected:

        // Shared pointer to AbstractMaster
        std::shared_ptr<AbstractMaster> m_p_master;
};

#endif // ABSTRACTCONTROLLER_H
