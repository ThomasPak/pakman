#include <string>
#include <fstream>

#include "common.h"
#include "read_input.h"
#include "LongOptions.h"
#include "Arguments.h"

#include "SweepController.h"

// Static help function
std::string SweepController::help()
{
    std::string message;
    message +=
R"(* Help message for 'sweep' controller *

Description:
  The sweep method interprets the stdout of GENERATOR as newline-separated list
  of parameters and runs SIMULATOR on each of them.

  Upon completion, the controller outputs the parameter names, followed by
  newline-separated list of simulated parameters.

Required arguments:
  INPUT_FILE                input file

Contents of input file:
  SIMULATOR                 simulator command
  PARAMETER_NAMES           comma-separated list of parameter names
  GENERATOR                 generator command

Usage:
  )";
    message += program_name;
    message +=
R"( <master> sweep INPUT_FILE [optional args]...
)";

    return message;
}

void SweepController::addLongOptions(LongOptions& lopts)
{
    // No SweepController-specific options to add
}

SweepController* SweepController::makeController(const Arguments& args)
{
    // Check if correct number of positional arguments were given
    if (args.numberOfPositionalArguments() < 1)
    {
        std::runtime_error e("Insufficient required arguments were given.");
        throw e;
    }
    else if (args.numberOfPositionalArguments() > 1)
    {
        std::runtime_error e("Too many required arguments were given.");
        throw e;
    }

    // Open
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

    // Parse stdin and store in input_obj
    sweep::input_t input_obj;
    sweep::read_input(input_file, input_obj);

    // Make SweepController
    return new SweepController(input_obj);
}
