#include <memory>
#include <fstream>
#include <string>
#include <chrono>
#include <random>

#include "common.h"
#include "read_input.h"
#include "LongOptions.h"
#include "Arguments.h"

#include "ABCSMCController.h"

std::string ABCSMCController::help()
{
    std::string message;
    message +=
R"(* Help message for 'smc' controller *

Description:
  The ABC sequential Monte Carlo (SMC) method evolves a population of
  parameters through multiple generations to an approximate posterior
  distribution, with epsilon decreasing at each generation.  The generations
  are numbered starting from 0.

  The sequence of epsilon values is given by the comma-separated list EPSILONS.
  The number of values in EPSILONS determines the number of generations.

  The first generation of candidate parameters is obtained from the stdout of
  PRIOR_SAMPLER.

  In subsequent generations, candidate parameters are sampled by perturbing a
  parameter from the previous generation with PERTURBER.  PERTURBER accepts two
  lines as its input; the first line contains the current generation 'T' and
  the second line contains the parameter to be perturbed.

  The parameter to be perturbed is sampled from the previous generation with
  weights that are calculated using PRIOR_PDF and PERTURBATION_PDF.  PRIOR_PDF
  accepts a parameter on its stdin and returns the corresponding prior
  probability density on its stdout.

  PERTURBATION_PDF accepts at least two lines on its stdin, containing the
  current generation 'T' and the perturbed parameter, respectively.  This is
  then followed by an arbitrary number of parameters, each on a separate line.
  For every parameter from the third line onwards, PERTURBATION_PDF outputs on
  its stdout the corresponding probability density for reaching the perturbed
  parameter by perturbing the given parameter.

  For every candidate parameter, SIMULATOR is invoked and given two lines as
  its input; the first line contains the current epsilon value and the second
  line contains the candidate parameter.

  The output of SIMULATOR indicates whether the parameter was accepted or
  rejected; an output of '0' means that the parameter was rejected and an
  output of '1' means that the parameter was accepted.

  Upon completion, the controller outputs the parameter names in
  PARAMETER_NAMES, followed by a newline-separated list of accepted parameters.

Required arguments:
  INPUT_FILE                input file
  N                         population size

Contents of input file:
  EPSILONS                     comma-separated list of epsilons
  SIMULATOR                    simulator command
  PARAMETER_NAMES              comma-separated list of parameter names
  PRIOR_SAMPLER                prior sampler command
  PERTURBER                    perturber command
  PRIOR_PDF                    prior pdf command
  PERTURBATION_PDF             perturbation pdf command

Usage:
  )";
    message += program_name;
    message +=
R"( <master> smc INPUT_FILE N [optional args]...
)";

    return message;
}

void ABCSMCController::addLongOptions(
        LongOptions& lopts)
{
    // No ABCSMCController-specific options to add
}

ABCSMCController* ABCSMCController::makeController(const Arguments& args)
{
    // Check if correct number of positional arguments were given
    if (args.numberOfPositionalArguments() < 2)
    {
        std::runtime_error e("Insufficient required arguments were given.");
        throw e;
    }
    else if (args.numberOfPositionalArguments() > 2)
    {
        std::runtime_error e("Too many required arguments were given.");
        throw e;
    }

    // Open input file
    std::ifstream input_file(args.positionalArgument(0));
    if (!input_file.good())
    {
        std::string error_string;
        error_string += "An error occured while opening ";
        error_string += args.positionalArgument(0);
        error_string += ".\n";
        std::runtime_error e(error_string);
        throw e;
    }

    // Read population size
    int pop_size = std::stoi(args.positionalArgument(1));
    if (pop_size <= 0)
    {
        std::runtime_error e("Population size "
                "must be positive integer.\n");
        throw e;
    }

    // Parse stdin and store in input_obj
    smc::input_t input_obj;
    smc::read_input(input_file, input_obj);

    // Create random number generator
    // TODO accept other seeds
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::shared_ptr<std::default_random_engine> p_generator =
        std::make_shared<std::default_random_engine>(seed);

    // Make ABCSMCController
    return new ABCSMCController(input_obj, p_generator, pop_size);
}
