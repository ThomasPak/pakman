#include <string>
#include <iostream>

#include "master/AbstractMaster.h"
#include "controller/AbstractController.h"

#include "help.h"

std::string usage()
{
    std::string usage_string;
    usage_string += "Usage: ";
    usage_string += g_program_name;
    usage_string += " <master> <controller>";
    usage_string += " <required args>... [options]...";
    usage_string += "\n";
    return usage_string;
}

std::string general_options()
{
    return R"(General options:
  -h, --help                    show help message
  -i, --ignore-errors           ignore nonzero return code from simulator
  -d, --discard-child-stderr    discard stderr from child processes
  -v, --verbosity=level         set verbosity level to debug/info/off
                                (default info)
  -o, --output-file             set output file (default stdout)
)";
}

void overview(int status)
{
	// Print usage
	std::cout << usage();

    // Print general options
    std::cout << std::endl;
    std::cout << general_options();

	// Print overview
    std::cout << std::endl;
    std::cout <<
R"(Available masters:
  serial        run at most one simulation overall
  mpi           run at most one simulation per launched MPI process
See ')" << g_program_name << R"( <master> --help' for more info.

Available controllers:
  sweep         run a parameter sweep
  rejection     run the ABC rejection algorithm
  smc           run the ABC SMC algorithm
See ')" << g_program_name << R"( <controller> --help' for more info.

Alternatively, see ')" <<
g_program_name << R"( <master> <controller> --help' for more info
on both <master> and <controller>.
)";

	// Exit
	exit(status);
}

void help(master_t master, controller_t controller, int status)
{
	// Print usage
	std::cout << usage();

    // Print general options
    std::cout << std::endl;
    std::cout << general_options();

	// If given valid master, print master help
	if (master != no_master)
	{
        // Leave one line
        std::cout << std::endl;

        // Print master help
        std::cout << AbstractMaster::help(master);
	}

	// If given valid controller, print controller help
	if (controller != no_controller)
	{
        // Leave one line
        std::cout << std::endl;

        // Print master help
        std::cout << AbstractController::help(controller);
	}

    // Exit
	exit(status);
}
