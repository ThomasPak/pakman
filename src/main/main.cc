#include <iostream>
#include <string>
#include <exception>
#include <chrono>

#include <getopt.h>
#include <libgen.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "core/common.h"
#include "help.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "core/OutputStreamHandler.h"

#include "master/AbstractMaster.h"
#include "controller/AbstractController.h"

// Program name
const char *g_program_name;

// Global variables
std::chrono::milliseconds g_main_timeout(1);
std::chrono::milliseconds g_kill_timeout(100);

bool g_ignore_errors = false;
bool g_force_host_spawn = false;
bool g_discard_child_stderr = false;

bool g_program_terminated = false;

std::string g_output_file;

// Is help flag
bool is_help_flag(const std::string& flag)
{
    return (flag.compare("-h") == 0) || (flag.compare("--help") == 0);
}

// Add general options
void add_general_long_options(LongOptions& lopts)
{
    lopts.add({"help", no_argument, nullptr, 'h'});
    lopts.add({"ignore-errors", no_argument, nullptr, 'i'});
    lopts.add({"discard-child-stderr", no_argument, nullptr, 'd'});
    lopts.add({"verbosity", required_argument, nullptr, 'v'});
    lopts.add({"output-file", required_argument, nullptr, 'o'});
}

// Process general options
void process_general_options(master_t master, controller_t controller,
        const Arguments& args)
{
    if (args.isOptionalArgumentSet("help"))
        help(master, controller, EXIT_SUCCESS);

    if (args.isOptionalArgumentSet("ignore-errors"))
        g_ignore_errors = true;

    if (args.isOptionalArgumentSet("discard-child-stderr"))
        g_discard_child_stderr = true;

    if (args.isOptionalArgumentSet("verbosity"))
    {
        std::string arg = args.optionalArgument("verbosity");

        if (arg.compare("info") == 0)
            spdlog::set_level(spdlog::level::info);
        else if (arg.compare("debug") == 0)
            spdlog::set_level(spdlog::level::debug);
        else if (arg.compare("off") == 0)
            spdlog::set_level(spdlog::level::off);
        else
            help(master, controller, EXIT_FAILURE);
    }

    if (args.isOptionalArgumentSet("output-file"))
        g_output_file = args.optionalArgument("output-file");
}

int main(int argc, char *argv[])
{
    // Set g_program_name
    g_program_name = basename(argv[0]);

    // Set logger
    auto stderr_console = spdlog::stderr_color_st(g_program_name);
    spdlog::set_default_logger(stderr_console);
    spdlog::set_level(spdlog::level::info);

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

    // Initialize LongOptions object
    LongOptions lopts;

    // Add general, master and controller options
    add_general_long_options(lopts);
    AbstractMaster::addLongOptions(master, lopts);
    AbstractController::addLongOptions(controller, lopts);

    // Create Arguments object
    // Set optind to 3 such that getopt skips the first 2 arguments
    optind = 3;
    Arguments args(lopts, argc, argv);

    // Process general options
    process_general_options(master, controller, args);

    // Execute appropriate run function
    try
    {
        AbstractMaster::run(master, controller, args);
    }
    catch (const std::exception& e)
    {
        // Print error message
        std::string error_msg;
        error_msg += "An exception occurred while running ";
        error_msg += g_program_name;
        error_msg += "!\n";
        error_msg += "  what(): ";
        error_msg += e.what();
        error_msg += "\n";
        std::cerr << error_msg;

        // Clean up
        AbstractMaster::cleanup(master);
        OutputStreamHandler::destroy();

        // Return nonzero exit code
        return EXIT_FAILURE;
    }

    // Cleanup
    OutputStreamHandler::destroy();

    // Exit
    return 0;
}
