#include <memory>
#include <fstream>
#include <string>
#include <random>

#include "core/common.h"
#include "core/utils.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "interface/input.h"

#include "AdaptiveABCSMCController.h"

std::string AdaptiveABCSMCController::help()
{
    return
R"(* Help message for 'adaptive_smc' controller *

Description:
  [INSERT DESCRIPTION]

Required arguments:
  [INSERT REQUIRED ARGUMENTS]

Adaptive ABC SMC controller options:
  [INSERT OPTIONS]
)";
}

void AdaptiveABCSMCController::addLongOptions(
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

AdaptiveABCSMCController* AdaptiveABCSMCController::makeController(const Arguments& args)
{
    Input input_obj;

    // Parse command-line options
    input_obj = Input::makeInput(args);

    // Make AdaptiveABCSMCController
    return new AdaptiveABCSMCController(input_obj);
}

// Construct Input from Arguments object
AdaptiveABCSMCController::Input AdaptiveABCSMCController::Input::makeInput(
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
        error_msg += " adaptive_smc --help' for more info";
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
        error_msg += " adaptive_smc --help' for more info";
        throw std::runtime_error(error_msg);
    }

    return input_obj;
}
