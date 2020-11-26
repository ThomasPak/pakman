#include <memory>
#include <fstream>
#include <string>
#include <random>

#include "core/common.h"
#include "core/utils.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "interface/input.h"

#include "ABCSMCController.h"

std::string ABCSMCController::help()
{
    return
R"(* Help message for 'smc' controller *

Description:
  The ABC sequential Monte Carlo (SMC) method evolves a population of
  parameters through multiple generations to an approximate posterior
  distribution, with epsilon decreasing at each generation.  The generations
  are numbered starting from 0.

  The sequence of epsilon values is given by the comma-separated list
  'epsilons'.  The number of values in 'epsilons' determines the number of
  generations.

  The first generation of candidate parameters is obtained from the stdout of
  'prior_sampler'.

  In subsequent generations, candidate parameters are sampled by perturbing a
  parameter from the previous generation with 'perturber'.  'perturber' accepts
  two lines as its input; the first line contains the current generation 't'
  and the second line contains the parameter to be perturbed.

  The parameter to be perturbed is sampled from the previous generation with
  weights that are calculated using 'prior_pdf' and 'perturbation_pdf'.
  'prior_pdf' accepts a parameter on its stdin and returns the corresponding
  prior probability density on its stdout.

  'perturbation_pdf' accepts at least two lines on its stdin, containing the
  current generation 't' and the perturbed parameter, respectively.  This is
  then followed by an arbitrary number of parameters, each on a separate line.
  For every parameter from the third line onwards, 'perturbation_pdf' outputs
  on its stdout the corresponding probability density for reaching the
  perturbed parameter by perturbing the given parameter.

  For every candidate parameter, 'simulator' is invoked and given two lines as
  its input; the first line contains the current epsilon value and the second
  line contains the candidate parameter.

  The output of 'simulator' indicates whether the parameter was accepted or
  rejected; an output of '0', 'reject' or 'rejected' means that the parameter
  was rejected and an output of '1', 'accept' or 'accepted' means that the
  parameter was accepted.

  Upon completion, the controller outputs the parameter names, followed by
  newline-separated list of accepted parameters.

Required arguments:
  -N, --population-size=NUM     NUM is the parameter population size
  -E, --epsilons=EPS            EPS is comma-separated list of tolerances
                                that are passed to simulator
  -P, --parameter-names=NAMES   NAMES is comma-separated list of
                                parameter names
  -S, --simulator=CMD           CMD is simulator command
  -R, --prior-sampler=CMD       CMD is prior_sampler command
  -T, --perturber=CMD           CMD is perturber command
  -I, --prior-pdf=CMD           CMD is prior_pdf command
  -U, --perturbation-pdf=CMD    CMD is perturbation_pdf command

ABC SMC controller options:
  -s, --seed=SEED               SEED is the seed for the pseudo random number
                                generator that is used to sample from the
                                parameter population (by default, the seed is
                                derived from the system clock).
)";
}

void ABCSMCController::addLongOptions(
        LongOptions& lopts)
{
    lopts.add({"population-size", required_argument, nullptr, 'N'});
    lopts.add({"epsilons", required_argument, nullptr, 'E'});
    lopts.add({"parameter-names", required_argument, nullptr, 'P'});
    lopts.add({"simulator", required_argument, nullptr, 'S'});
    lopts.add({"prior-sampler", required_argument, nullptr, 'R'});
    lopts.add({"perturber", required_argument, nullptr, 'T'});
    lopts.add({"prior-pdf", required_argument, nullptr, 'I'});
    lopts.add({"perturbation-pdf", required_argument, nullptr, 'U'});
    lopts.add({"seed", required_argument, nullptr, 's'});
}

ABCSMCController* ABCSMCController::makeController(const Arguments& args)
{
    Input input_obj;

    // Parse command-line options
    input_obj = Input::makeInput(args);

    // Make ABCSMCController
    return new ABCSMCController(input_obj);
}

// Construct Input from Arguments object
ABCSMCController::Input ABCSMCController::Input::makeInput(
        const Arguments& args)
{
    // Initialize input
    Input input_obj;

    // Process optional arguments
    if (args.isOptionalArgumentSet("seed"))
    {
        input_obj.seed =
            parse_unsigned_long_integer(args.optionalArgument("seed"));
    }

    try
    {
        input_obj.population_size =
            parse_integer(args.optionalArgument("population-size"));

        input_obj.epsilons = parse_epsilons(args.optionalArgument("epsilons"));

        input_obj.parameter_names =
            parse_parameter_names(args.optionalArgument("parameter-names"));

        input_obj.simulator =
            parse_command(args.optionalArgument("simulator"));

        input_obj.prior_sampler =
            parse_command(args.optionalArgument("prior-sampler"));

        input_obj.perturber =
            parse_command(args.optionalArgument("perturber"));

        input_obj.prior_pdf =
            parse_command(args.optionalArgument("prior-pdf"));

        input_obj.perturbation_pdf =
            parse_command(args.optionalArgument("perturbation-pdf"));
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "Out of range: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "One or more arguments missing or incorrect, try '";
        error_msg += g_program_name;
        error_msg += " smc --help' for more info";
        throw std::runtime_error(error_msg);
    }
    catch (const std::invalid_argument& e)
    {
        std::string error_msg;
        error_msg += "  Invalid argument: ";
        error_msg += e.what();
        error_msg += '\n';
        error_msg += "One or more arguments missing or incorrect, try '";
        error_msg += g_program_name;
        error_msg += " smc --help' for more info";
        throw std::runtime_error(error_msg);
    }

    return input_obj;
}
