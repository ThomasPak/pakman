#include <chrono>

#include <getopt.h>
#include <libgen.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "common.h"
#include "help.h"

#include "AbstractMaster.h"
#include "AbstractController.h"

// Program name
const char *program_name;

// Global variables
std::chrono::milliseconds MAIN_TIMEOUT(1);
std::chrono::milliseconds KILL_TIMEOUT(100);

bool ignore_errors = false;
bool force_host_spawn = false;

bool program_terminated = false;

// Is help flag
bool is_help_flag(const std::string& flag)
{
    return (flag.compare("-h") == 0) || (flag.compare("--help") == 0);
}

int main(int argc, char *argv[])
{
    // Set program_name
    program_name = basename(argv[0]);

    // Set logger
    auto stderr_console = spdlog::stderr_color_st(program_name);
    spdlog::set_default_logger(stderr_console);
    spdlog::set_level(spdlog::level::debug);

    // If there are less than 3 arguments, print overview help
    if (argc < 3)
        overview(EXIT_FAILURE);

    // Initialize master and controller
    master_t master = no_master;
    controller_t controller = no_controller;

    // If first argument is not master
    if ((master = AbstractMaster::getMaster(argv[1])) == no_master)
    {
        // If first argument is not controller either, print overview
        if ((controller = AbstractController::getController(argv[1]))
                == no_controller)
            overview(EXIT_FAILURE);

        // First argument is controller
        // Check if second argument is "-h" or "--help"
        if (is_help_flag(argv[2]))
            help(master, controller, EXIT_FAILURE);

        // Else print overview
        else
            overview(EXIT_FAILURE);
    }

    // Reaching this part of the code means first argument is valid master
    // If second argument is not controller
    if ((controller = AbstractController::getController(argv[2]))
            == no_controller)
    {
        // Check if second argument is "-h" or "--help"
        if (is_help_flag(argv[2]))
            help(master, controller, EXIT_FAILURE);

        // Else print overview
        else
            overview(EXIT_FAILURE);
    }

    /* Reaching this part of the code means that the first argument was a valid
     * master and the second argument was a valid controller */

    // Set optind to 3 such that getopt skips the first 2 arguments
    optind = 3;

    // Execute appropriate run function
    AbstractMaster::run(master, controller, argc, argv);

    // Exit
    return 0;
}
