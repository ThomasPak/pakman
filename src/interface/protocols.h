#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include <string>
#include <vector>

#include "interface/types.h"

class Command;

/** @file protocols.h
 *
 * This file defines functions to format input to user executables and parse
 * output from user executables according to the [Pakman user executable
 * API](https://github.com/ThomasPak/pakman/wiki/User-executable-API).
 *
 * In addition, this file contains convenience functions that combine
 * formatting and parsing.
 */

/** Format input to simulator.
 *
 * @param epsilon  distance tolerance.
 * @param parameter  parameter to simulate.
 *
 * @return input string to simulator.
 */
std::string format_simulator_input(
        const Epsilon& epsilon,
        const Parameter& parameter);

/** Parse output from simulator.
 *
 * @param simulator_output  output string from simulator.
 *
 * @return whether parameter was accepted.
 */
bool parse_simulator_output(const std::string& simulator_output);

/** Parse output from prior_sampler.
 *
 * @param prior_sampler_output  output string from prior_sampler.
 *
 * @return parameter sampled from prior.
 */
Parameter parse_prior_sampler_output(const std::string& prior_sampler_output);

/** Format input to perturber.
 *
 * @param t  current generation.
 * @param source_parameter  source parameter to be perturbed.
 *
 * @return input string to perturber.
 */
std::string format_perturber_input(int t, const Parameter& source_parameter);

/** Parse output from perturber.
 *
 * @param perturber_output  output string from perturber.
 *
 * @return perturbed parameter.
 */
Parameter parse_perturber_output(const std::string& perturber_output);

/** Format input to prior_pdf.
 *
 * @param parameter  parameter to evaluate.
 *
 * @return input string to prior_pdf.
 */
std::string format_prior_pdf_input(const Parameter& parameter);

/** Parse output from prior_pdf.
 *
 * @param prior_pdf_output  output string from prior_pdf.
 *
 * @return prior probability density of parameter.
 */
double parse_prior_pdf_output(const std::string& prior_pdf_output);

/** Format input to perturbation_pdf.
 *
 * @param t  current generation.
 * @param perturbed_parameter  perturbed parameter.
 * @param parameter_population  parameter population.
 *
 * @return input string to perturbation_pdf.
 */
std::string format_perturbation_pdf_input(
        int t,
        const Parameter& perturbed_parameter,
        const std::vector<Parameter>& parameter_population);

/** Parse output from perturbation_pdf.
 *
 * @param perturbation_pdf_output  output string from perturbation_pdf.
 *
 * @return perturbation kernel probability densities for perturbed parameter
 * and parameter population.
 */
std::vector<double> parse_perturbation_pdf_output(
        const std::string& perturbation_pdf_output);

/** Parse output from generator.
 *
 * @param generator_output  output string from generator.
 *
 * @return list of parameters to simulate.
 */
std::vector<Parameter> parse_generator_output(
        const std::string& generator_output);

/** Sample from prior.
 *
 * @param prior_sampler  command to sample from prior.
 *
 * @return parameter sampled from prior.
 */
Parameter sample_from_prior(const Command& prior_sampler);

/** Perturb parameter.
 *
 * @param perturber  command to perturb parameter.
 * @param t  current generation.
 * @param source_parameter  source parameter to be perturbed.
 *
 * @return perturbed parameter.
 */
Parameter perturb_parameter(const Command& perturber, int t, Parameter
        source_parameter);

/** Get prior probability density function.
 *
 * @param prior_pdf  command to get prior pdf.
 * @param parameter  parameter to evaluate.
 *
 * @return prior probability density of parameter.
 */
double get_prior_pdf(const Command& prior_pdf, Parameter parameter);

/** Get perturbation probability density function.
 *
 * @param perturbation_pdf  command to get perturbation pdf.
 * @param t  current generation.
 * @param perturbed_parameter  perturbed parameter.
 * @param parameter_population  parameter population.
 *
 * @return perturbation kernel probability densities for perturbed parameter
 * and parameter population.
 */
std::vector<double> get_perturbation_pdf(const Command& perturbation_pdf,
        int t, const Parameter& perturbed_parameter,
        const std::vector<Parameter>& parameter_population);

#endif // PROTOCOLS_H
