#include <fstream>
#include <string>

#include "core/common.h"
#include "interface/read_input.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"

#include "ABCRejectionController.h"

// Static help function
std::string ABCRejectionController::help()
{
    std::string message;
    message +=
R"(* Help message for 'rejection' controller *

Description:
  The ABC rejection method samples candidate parameters from the stdout of
  PRIOR_SAMPLER.  For every candidate parameter, SIMULATOR is invoked and given
  two lines as its input; the first line contains EPSILON and the second line
  contains the candidate parameter.

  The output of SIMULATOR indicates whether the parameter was accepted or
  rejected; an output of '0' means that the parameter was rejected and an
  output of '1' means that the parameter was accepted.

  Upon completion, the controller outputs the parameter names in
  PARAMETER_NAMES, followed by a newline-separated list of accepted parameters.

Required arguments:
  INPUT_FILE                input file
  N                         number of parameters to accept

Contents of input file:
  EPSILON                   value of epsilon
  SIMULATOR                 simulator command
  PARAMETER_NAMES           comma-separated list of parameter names
  PRIOR_SAMPLER             prior sampler command

Usage:
  )";
    message += program_name;
    message +=
R"( <master> rejection INPUT_FILE N [optional args]...
)";

    return message;
}

void ABCRejectionController::addLongOptions(
        LongOptions& lopts)
{
    // No ABCRejectionController-specific options to add
}

// Static function to make from positional arguments
ABCRejectionController* ABCRejectionController::makeController(
        const Arguments& args)
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

    // Read number of accepted parameters
    int num_accept = std::stoi(args.positionalArgument(1));
    if (num_accept <= 0)
    {
        std::runtime_error e("Number of accepted parameters "
                "must be positive integer.\n");
        throw e;
    }

    // Parse stdin and store in input_obj
    rejection::input_t input_obj;
    rejection::read_input(input_file, input_obj);

    return new ABCRejectionController(input_obj, num_accept);
}
