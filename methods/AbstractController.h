#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

#include <memory>

#include "AbstractMaster.h"

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

    protected:

        // Shared pointer to AbstractMaster
        std::shared_ptr<AbstractMaster> m_p_master;
};

#endif // ABSTRACTCONTROLLER_H
