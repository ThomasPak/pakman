#include <string>
#include <vector>

#include "core/utils.h"
#include "core/Command.h"

#include "input.h"

int parse_integer(const std::string& raw_input)
{
    return std::stoi(raw_input);
}

unsigned long parse_unsigned_long_integer(const std::string& raw_input)
{
    return std::stoul(raw_input);
}

Command parse_command(const std::string& raw_input)
{
    return static_cast<Command>(raw_input);
}

Epsilon parse_epsilon(const std::string& raw_input)
{
    return static_cast<Epsilon>(raw_input);
}

std::vector<Epsilon> parse_epsilons(const std::string& raw_input)
{
    // Get raw strings
    std::vector<std::string> tokens = parse_tokens(raw_input, ",");

    // Construct Epsilon vector
    std::vector<Epsilon> epsilons;
    for (std::string& token : tokens)
        epsilons.push_back(std::move(token));

    return epsilons;
}

std::vector<ParameterName> parse_parameter_names(const std::string& raw_input)
{
    // Get raw strings
    std::vector<std::string> tokens = parse_tokens(raw_input, ",");

    // Construct ParameterName vector
    std::vector<ParameterName> parameter_names;
    for (std::string& token : tokens)
        parameter_names.push_back(std::move(token));

    return parameter_names;
}
