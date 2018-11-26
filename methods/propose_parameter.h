#ifndef PROPOSE_PARAMETER_H
#define PROPOSE_PARAMETER_H

#include "types.h"

void propose_parameter(const cmd_t& proposer,
                       const parameter_t& old_prmtr,
                       parameter_t& new_prmtr);

#endif // PROPOSE_PARAMETER_H
