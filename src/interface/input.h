#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>

#include "core/Command.h"
#include "types.h"

int parse_integer(const std::string& raw_input);
Command parse_command(const std::string& raw_input);
Epsilon parse_epsilon(const std::string& raw_input);

std::vector<Epsilon> parse_epsilons(const std::string& raw_input);
std::vector<ParameterName> parse_parameter_names(const std::string& raw_input);

#endif // INPUT_H
