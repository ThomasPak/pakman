#ifndef WRITE_PARAMETERS_H
#define WRITE_PARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include "core/Parameter.h"

void write_parameters(std::ostream& ostrm,
                      const std::vector<std::string>& parameter_names,
                      const std::vector<Parameter>& parameters);

#endif // WRITE_PARAMETERS_H
