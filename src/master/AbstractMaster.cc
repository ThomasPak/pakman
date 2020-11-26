#include <string>
#include <memory>

#include <assert.h>

#include "AbstractMaster.h"

// Construct from pointer to program terminated flag
AbstractMaster::AbstractMaster(bool *p_program_terminated) :
    m_p_program_terminated(p_program_terminated)
{
}

// Assign controller
void AbstractMaster::assignController(
        std::shared_ptr<AbstractController> p_controller)
{
    m_p_controller = p_controller;
}

// Getter for m_p_program_terminated
bool AbstractMaster::programTerminated() const
{
    return *m_p_program_terminated;
}
