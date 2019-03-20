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
    // Extract line
    std::string line;
    std::stringstream sstrm(simulator_output);
    std::getline(sstrm, line);

    // Ensure that end of input has been reached
    if (sstrm.eof() || (sstrm.peek() != EOF))
    {
        std::string error_msg;
        error_msg += "Simulator output must contain exactly one "
            "newline-terminated line, given output: ";
        error_msg += simulator_output;
        throw std::runtime_error(error_msg);
    }

    // Parse line
    if (line.compare("1") == 0)
        return true;
    else if (line.compare("accept") == 0)
        return true;
    else if (line.compare("accepted") == 0)
        return true;
    if (line.compare("0") == 0)
        return false;
    else if (line.compare("reject") == 0)
        return false;
    else if (line.compare("rejected") == 0)
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
    // Extract line
    std::string line;
    std::stringstream sstrm(prior_sampler_output);
    std::getline(sstrm, line);

    // Ensure that end of input has been reached
    if (sstrm.eof() || (sstrm.peek() != EOF))
    {
        std::string error_msg;
        error_msg += "Prior_sampler output must contain exactly one "
            "newline-terminated line, given output: ";
        error_msg += prior_sampler_output;
        throw std::runtime_error(error_msg);
    }

    // Parse line
    try
    {
        return line;
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

// prior_pdf protocol
std::string format_prior_pdf_input(const Parameter& parameter)
{
    std::string input_string;
    input_string += parameter.str();
    input_string += '\n';

    return input_string;
}

double parse_prior_pdf_output(const std::string& prior_pdf_output)
{
    // Extract line
    std::string line;
    std::stringstream sstrm(prior_pdf_output);
    std::getline(sstrm, line);

    // Ensure that end of input has been reached
    if (sstrm.eof() || (sstrm.peek() != EOF))
    {
        std::string error_msg;
        error_msg += "Prior_pdf output must contain exactly one "
            "newline-terminated line, given output: ";
        error_msg += prior_pdf_output;
        throw std::runtime_error(error_msg);
    }

    // Parse line as double-precision floating point
    try
    {
        return std::stod(line);
    }
    catch (std::invalid_argument& e)
    {
        std::string error_msg;
        error_msg += "Cannot parse output of prior_pdf: ";
        error_msg += prior_pdf_output;
        error_msg += '\n';
        error_msg += "Invalid argument: ";
        error_msg += e.what();
        throw std::runtime_error(error_msg);
    }
    catch (std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "Cannot parse output of prior_pdf: ";
        error_msg += prior_pdf_output;
        error_msg += '\n';
        error_msg += "Out of range: ";
        error_msg += e.what();
        throw std::runtime_error(error_msg);
    }
}
