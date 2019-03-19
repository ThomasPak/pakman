#ifndef WRITE_PARAMETERS_H
#define WRITE_PARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include "Parameter.h"
#include "types.h"

void write_parameters(std::ostream& ostrm,
                      const std::vector<std::string>& parameter_names,
                      const std::vector<Parameter>& parameters);

#endif // WRITE_PARAMETERS_H
