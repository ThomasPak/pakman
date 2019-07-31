#ifndef WRITE_PARAMETERS_H
#define WRITE_PARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include "types.h"

/** @file output.h
 *
 * This file contains functions to format the output of Pakman.
 */

/** Write parameters to output stream.
 *
 * @param ostrm  output stream.
 * @param parameter_names  list of parameter names.
 * @param parameters  list of parameters.
 */
void write_parameters(std::ostream& ostrm,
        const std::vector<ParameterName>& parameter_names,
        const std::vector<Parameter>& parameters);

#endif // WRITE_PARAMETERS_H
