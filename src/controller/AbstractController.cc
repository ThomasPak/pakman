#include <memory>

#include "AbstractController.h"

// Assign Master
void AbstractController::assignMaster(std::shared_ptr<AbstractMaster> p_master)
{
    m_p_master = p_master;
}
