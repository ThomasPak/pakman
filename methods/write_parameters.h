#ifndef WRITE_PARAMETERS_H
#define WRITE_PARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include "types.h"

void write_parameters(std::ostream& ostrm,
                      const std::vector<std::string>& parameter_names,
                      const std::vector<parameter_t>& parameters);

#endif // WRITE_PARAMETERS_H
