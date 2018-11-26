#ifndef ACCEPTANCE_PROBABILITY_H
#define ACCEPTANCE_PROBABILITY_H

#include "types.h"

double acceptance_probability(const cmd_t& prior_pdf,
                              const cmd_t& proposal_pdf,
                              const parameter_t& old_prmtr,
                              const parameter_t& new_prmtr);

#endif // ACCEPTANCE_PROBABILITY_H
