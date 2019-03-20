#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "Parameter.h"

#include "protocols.h"

// simulator protocol
std::string format_simulator_input(const std::string& epsilon, const Parameter& parameter)
{
    std::string input_string;
    input_string += epsilon;
    input_string += '\n';
    input_string += parameter.str();
    input_string += '\n';

    return input_string;
}

bool parse_simulator_output(const std::string& simulator_output)
{
    // Extract result
    std::string result;
    std::getline(std::stringstream(simulator_output), result);

    // Parse result
    if (result.compare("1") == 0)
        return true;
    else if (result.compare("accept") == 0)
        return true;
    else if (result.compare("accepted") == 0)
        return true;
    if (result.compare("0") == 0)
        return false;
    else if (result.compare("reject") == 0)
        return false;
    else if (result.compare("rejected") == 0)
        return false;
    // Invalid simulator output
    else
    {
        std::string error_msg;
        error_msg += "Cannot parse output of simulator: ";
        error_msg += simulator_output;
        throw std::runtime_error(error_msg);
    }
}

// prior_sampler protocol
Parameter parse_prior_sampler_output(const std::string& prior_sampler_output)
{
    try
    {
        return prior_sampler_output;
    }
    catch (std::runtime_error& e)
    {
        std::string error_msg;
        error_msg += "Cannot parse output of prior_sampler: ";
        error_msg += prior_sampler_output;
        throw std::runtime_error(error_msg);
    }
}

// perturber protocol
std::string format_perturber_input(int t, const Parameter& parameter)
{
    std::string input_string;
    input_string += std::to_string(t);
    input_string += '\n';
    input_string += parameter.str();
    input_string += '\n';

    return input_string;
}

Parameter parse_perturber_output(const std::string& perturber_output)
{
    try
    {
        return perturber_output;
    }
    catch (std::runtime_error& e)
    {
        std::string error_msg;
        error_msg += "Cannot parse output of perturber: ";
        error_msg += perturber_output;
        throw std::runtime_error(error_msg);
    }
}
