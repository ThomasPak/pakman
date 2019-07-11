#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include <string>
#include <vector>

#include "core/Command.h"
#include "interface/types.h"

// simulator protocol
std::string format_simulator_input(
        const Epsilon& epsilon,
        const Parameter& parameter);
bool parse_simulator_output(const std::string& simulator_output);

// prior_sampler protocol
Parameter parse_prior_sampler_output(const std::string& prior_sampler_output);

// perturber protocol
std::string format_perturber_input(int t, const Parameter& parameter);
Parameter parse_perturber_output(const std::string& perturber_output);

// prior_pdf protocol
std::string format_prior_pdf_input(const Parameter& parameter);
double parse_prior_pdf_output(const std::string& prior_pdf_output);

// perturbation_pdf protocol
std::string format_perturbation_pdf_input(
        int t,
        const Parameter& perturbed_parameter,
        const std::vector<Parameter>& parameter_population);
std::vector<double> parse_perturbation_pdf_output(
        const std::string& perturbation_pdf_output);

// generator protocol
std::vector<Parameter> parse_generator_output(
        const std::string& generator_output);

// Call prior_sampler to sample from prior
Parameter sample_from_prior(const Command& prior_sampler);

// Call perturber to perturb parameter
Parameter perturb_parameter(const Command& perturber, int t, Parameter
        source_parameter);

// Call prior_pdf to get prior pdf of parameter
double get_prior_pdf(const Command& prior_pdf, Parameter parameter);

// Call perturbation_pdf to get perturbation pdf of parameters
std::vector<double> get_perturbation_pdf(const Command& perturbation_pdf, int t, const
        Parameter& perturbed_parameter, const std::vector<Parameter>&
        parameter_population);

#endif // PROTOCOLS_H
