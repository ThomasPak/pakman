#include <string>
#include <fstream>

#include "core/common.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/Command.h"
#include "interface/input.h"

#include "SweepController.h"

// Static help function
std::string SweepController::help()
{
    return
R"(* Help message for 'sweep' controller *

Description:
  The sweep method interprets the stdout of 'generator' as newline-separated
  list of parameters and runs 'simulator' on each of them.

  Upon completion, the controller outputs the parameter names, followed by
  newline-separated list of simulated parameters.

Required arguments:
  -P, --parameter-names=NAMES   NAMES is a comma-separated list of
                                parameter names
  -S, --simulator=CMD           CMD is simulator command
  -G, --generator=CMD           CMD is generator command
)";
}

void SweepController::addLongOptions(LongOptions& lopts)
{
    lopts.add({"parameter-names", required_argument, nullptr, 'P'});
    lopts.add({"simulator", required_argument, nullptr, 'S'});
    lopts.add({"generator", required_argument, nullptr, 'G'});
}

SweepController* SweepController::makeController(const Arguments& args)
{
    Input input_obj;

    // Parse command-line options
    input_obj = Input::makeInput(args);

    // Make SweepController
    return new SweepController(input_obj);
}

// Construct Input from Arguments object
SweepController::Input SweepController::Input::makeInput(const Arguments& args)
{
    // Initialize input
    Input input_obj;

    try
    {
        input_obj.simulator =
            parse_command(args.optionalArgument("simulator"));

        input_obj.parameter_names =
            parse_parameter_names(args.optionalArgument("parameter-names"));

        input_obj.generator =
            parse_command(args.optionalArgument("generator"));
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "One or more arguments missing, try '";
        error_msg += g_program_name;
        error_msg += " sweep --help' for more info";
        throw std::runtime_error(error_msg);
    }

    return input_obj;
}
