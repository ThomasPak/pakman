#include <fstream>
#include <string>

#include "core/common.h"
#include "core/utils.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"

#include "ABCRejectionController.h"

// Static help function
std::string ABCRejectionController::help()
{
    return
R"(* Help message for 'rejection' controller *

Description:
  The ABC rejection method samples candidate parameters from the stdout of
  'prior_sampler'.  For every candidate parameter, 'simulator' is invoked and
  given two lines as its input; the first line contains 'epsilon' and the
  second line contains the candidate parameter.

  The output of 'simulator' indicates whether the parameter was accepted or
  rejected; an output of '0', 'reject' or 'rejected' means that the parameter
  was rejected and an output of '1', 'accept' or 'accepted' means that the
  parameter was accepted.

  Upon completion, the controller outputs the parameter names, followed by
  newline-separated list of accepted parameters.

Required arguments:
  -N, --number-accept=NUM       NUM is number of parameters to accept
  -E, --epsilon=EPS             EPS is the tolerance passed to 'simulator'
  -P, --parameter-names=NAMES   NAMES is a comma-separated list of
                                parameter names
  -S, --simulator=CMD           CMD is simulator command
  -R, --prior-sampler=CMD       CMD is prior_sampler command
)";
}

void ABCRejectionController::addLongOptions(
        LongOptions& lopts)
{
    lopts.add({"number-accept", required_argument, nullptr, 'N'});
    lopts.add({"epsilon", required_argument, nullptr, 'E'});
    lopts.add({"parameter-names", required_argument, nullptr, 'P'});
    lopts.add({"simulator", required_argument, nullptr, 'S'});
    lopts.add({"prior-sampler", required_argument, nullptr, 'R'});
}

// Static function to make from positional arguments
ABCRejectionController* ABCRejectionController::makeController(
        const Arguments& args)
{
    Input input_obj;

    // Parse command-line options
    input_obj = Input::makeInput(args);

    // Make ABCRejectionController
    return new ABCRejectionController(input_obj);
}

// Construct Input from Arguments object
ABCRejectionController::Input ABCRejectionController::Input::makeInput(const Arguments& args)
{
    // Initialize input
    Input input_obj;

    try
    {
        input_obj.number_accept =
            std::stoi(args.optionalArgument("number-accept"));

        input_obj.epsilon = args.optionalArgument("epsilon");

        input_obj.parameter_names =
            parse_tokens(args.optionalArgument("parameter-names"), ",");

        input_obj.simulator = args.optionalArgument("simulator");
        input_obj.prior_sampler = args.optionalArgument("prior-sampler");
    }
    catch (const std::out_of_range& e)
    {
        std::string error_msg;
        error_msg += "One or more arguments missing, try '";
        error_msg += program_name;
        error_msg += " rejection --help' for more info";
        throw std::runtime_error(error_msg);
    }

    return input_obj;
}
