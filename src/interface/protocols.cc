#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "system/system_call.h"

#include "protocols.h"

// simulator protocol
std::string format_simulator_input(
        const Epsilon& epsilon,
        const Parameter& parameter)
{
    std::string input_string;
    input_string += epsilon.str();
    input_string += '\n';
    input_string += parameter.str();
    input_string += '\n';

    return input_string;
}

bool parse_simulator_output(const std::string& simulator_output)
{
    // Extract line
    std::string line;
    std::istringstream sstrm(simulator_output);
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
    std::istringstream sstrm(prior_sampler_output);
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
    catch (const std::runtime_error& e)
    {
        std::string error_msg;
        error_msg += e.what();
        error_msg += '\n';
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
    catch (const std::runtime_error& e)
    {
        std::string error_msg;
        error_msg += e.what();
        error_msg += '\n';
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
    std::istringstream sstrm(prior_pdf_output);
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
    catch (const std::invalid_argument& e)
    {
        std::string error_msg;
        error_msg += "Invalid argument: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Cannot parse output of prior_pdf: ";
        error_msg += prior_pdf_output;
        throw std::runtime_error(error_msg);
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "Out of range: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Cannot parse output of prior_pdf: ";
        error_msg += prior_pdf_output;
        throw std::runtime_error(error_msg);
    }
}

// perturbation_pdf protocol
std::string format_perturbation_pdf_input(
        int t,
        const Parameter& perturbed_parameter,
        const std::vector<Parameter>& parameter_population)
{
    std::string input_string;
    input_string += std::to_string(t);
    input_string += '\n';
    input_string += perturbed_parameter.str();
    input_string += '\n';

    for (const Parameter& parameter : parameter_population)
    {
        input_string += parameter.str();
        input_string += '\n';
    }

    return input_string;
}

std::vector<double> parse_perturbation_pdf_output(
        const std::string& perturbation_pdf_output)
{
    // Ensure that output ends with newline.  Cannot check for too many lines
    // because the number of lines is unspecified
    if (perturbation_pdf_output.back() != '\n')
    {
        std::string error_msg;
        error_msg += "Perturbation_pdf output must end with newline, "
            "given output: ";
        error_msg += perturbation_pdf_output;
        throw std::runtime_error(error_msg);
    }

    // Initialize sstrm and vector
    std::istringstream sstrm(perturbation_pdf_output);
    std::string line;
    std::vector<double> perturbation_pdf_vector;

    // Parse lines as double-precision floating point
    try
    {
        while (std::getline(sstrm, line))
            perturbation_pdf_vector.push_back(std::stod(line));
    }
    catch (const std::invalid_argument& e)
    {
        std::string error_msg;
        error_msg += "Invalid argument: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Cannot parse output of perturbation_pdf: ";
        error_msg += perturbation_pdf_output;
        throw std::runtime_error(error_msg);
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "Out of range: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Cannot parse output of perturbation_pdf: ";
        error_msg += perturbation_pdf_output;
        throw std::runtime_error(error_msg);
    }

    // Return vector
    return perturbation_pdf_vector;
}

// generator protocol
std::vector<Parameter> parse_generator_output(
        const std::string& generator_output)
{
    // Ensure that output ends with newline.  Cannot check for too many lines
    // because the number of lines is unspecified
    if (generator_output.back() != '\n')
    {
        std::string error_msg;
        error_msg += "Generator output must end with newline, "
            "given output: ";
        error_msg += generator_output;
        throw std::runtime_error(error_msg);
    }

    // Initialize sstrm and vector
    std::istringstream sstrm(generator_output);
    std::string line;
    std::vector<Parameter> generator_vector;

    // Parse lines as double-precision floating point
    try
    {
        while (std::getline(sstrm, line))
            generator_vector.push_back(std::move(line));
    }
    catch (const std::runtime_error& e)
    {
        std::string error_msg;
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "Cannot parse output of generator: ";
        error_msg += generator_output;
        throw std::runtime_error(error_msg);
    }

    // Return vector
    return generator_vector;
}


// Call prior_sampler to sample from prior
Parameter sample_from_prior(const Command& prior_sampler)
{
    std::string prior_sampler_output = system_call(prior_sampler);
    return parse_prior_sampler_output(prior_sampler_output);
}

// Call perturber to perturb parameter
Parameter perturb_parameter(const Command& perturber, int t, Parameter
        source_parameter)
{
        std::string perturber_input = format_perturber_input(t,
                source_parameter);
        std::string perturber_output = system_call(perturber, perturber_input);
        return parse_perturber_output(perturber_output);
}

// Call prior_pdf to get prior pdf of parameter
double get_prior_pdf(const Command& prior_pdf, Parameter parameter)
{
    std::string prior_pdf_input = format_prior_pdf_input(parameter);
    std::string prior_pdf_output = system_call(prior_pdf, prior_pdf_input);
    return parse_prior_pdf_output(prior_pdf_output);
}

// Call perturbation_pdf to get perturbation pdf of parameters
std::vector<double> get_perturbation_pdf(const Command& perturbation_pdf,
        int t, const Parameter& perturbed_parameter,
        const std::vector<Parameter>& parameter_population)
{
    std::string perturbation_pdf_input = format_perturbation_pdf_input(t,
            perturbed_parameter, parameter_population);
    std::string perturbation_pdf_output = system_call(perturbation_pdf,
            perturbation_pdf_input);
    return parse_perturbation_pdf_output(perturbation_pdf_output);
}
