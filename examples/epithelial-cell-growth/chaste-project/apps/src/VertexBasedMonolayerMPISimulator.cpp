#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "ExecutableSupport.hpp"
#include "Exception.hpp"
#include "PetscTools.hpp"
#include "PetscException.hpp"
#include "SimulationTime.hpp"
#include "OutputFileHandler.hpp"

#include "CellCycleTimesGenerator.hpp"
#include "CellPropertyRegistry.hpp"
#include "CellId.hpp"

#include "PakmanMPIWorker.hpp"
#include "VertexBasedMonolayerSimulation.hpp"

std::string temporary_chaste_directory();
void remove_temporary_chaste_directory(const std::string& tmp_dir);
int read_data_file(const std::string& data_file);
void setup_singletons();
void destroy_singletons();
int simulator(int argc, char *argv[], const std::string& input_string,
        std::string& output_string);

int main(int argc, char *argv[])
{
    // This sets up PETSc and prints out copyright information, etc.
    ExecutableSupport::StartupWithoutShowingCopyright(&argc, &argv);

    // Create MPI Worker
    PakmanMPIWorker worker(&simulator);

    // Run MPI Worker
    worker.run(argc, argv);

    // End by finalizing PETSc, and returning a suitable exit code.
    // 0 means 'no error'
    ExecutableSupport::FinalizePetsc();
    return ExecutableSupport::EXIT_OK;
}

int simulator(int argc, char *argv[], const std::string& input_string,
        std::string& output_string)
{
    // Temporary directory
    std::string tmp_dir;
    bool keep_testoutput = false;
    bool silent = false;

    // You should put all the main code within a try-catch, to ensure that
    // you clean up PETSc before quitting.
    try
    {
        // Check arguments
        if (argc < 2)
        {
            std::string error_msg;
            error_msg += "Usage: ";
            error_msg += argv[0];
            error_msg += " DATAFILE [--silent] [--keep-testoutput]";
            ExecutableSupport::PrintError(error_msg, true);

            // Write result
            output_string.assign("");

            // End by finalizing PETSc, and returning a suitable exit code.
            // 0 means 'no error'
            return ExecutableSupport::EXIT_BAD_ARGUMENTS;
        }

        // Check if silent or keep_testoutput flags are set
        for (int argi = 2; argi < argc; argi++)
        {
            if (std::string(argv[argi]).compare("--silent") == 0)
                silent = true;
            else if (std::string(argv[argi]).compare("--keep-testoutput") == 0)
                keep_testoutput = true;
        }

        // Get input
        int epsilon; double tcycle;
        std::stringstream inputstrm(input_string);
        inputstrm >> epsilon;
        inputstrm >> tcycle;

        // Print output
        if (!silent)
        {
            std::cerr << "Epsilon: " << epsilon << std::endl;
            std::cerr << "Tcycle: " << tcycle << std::endl;
        }

        // Get temporary directory
        tmp_dir = temporary_chaste_directory();
        if (!silent)
        {
            std::cerr << "Temporary directory: " <<
                (OutputFileHandler::GetChasteTestOutputDirectory() + tmp_dir)
                << std::endl;
        }

        // Prepare simulation
        setup_singletons();

        // Run simulation
        int sim_num_cells = VertexBasedMonolayerSimulation(tmp_dir, tcycle);

        destroy_singletons();

        // Read data
        int data_num_cells = read_data_file(argv[1]);

        if (!silent)
        {
            std::cerr << "Observed cell count: " << data_num_cells << std::endl;
            std::cerr << "Simulated cell count: " << sim_num_cells << std::endl;
        }

        // Initialize result
        std::string result;

        // Compare data and simulation and send result
        if (abs(sim_num_cells - data_num_cells) <= epsilon)
            // Parameter accepted
            result.assign("accept\n");
        else
            // Parameter rejected
            result.assign("reject\n");

        // Write result
        output_string.assign(result);

        // Remove temporary directory
        if (!keep_testoutput)
            remove_temporary_chaste_directory(tmp_dir);

        return ExecutableSupport::EXIT_OK;
    }
    catch (const Exception& e)
    {
        std::cerr << "Caught error!\n";
        ExecutableSupport::PrintError(e.GetMessage());

        // Ensure singletons are destroyed
        destroy_singletons();

        // Write result
        output_string.assign("");

        // Remove temporary directory
        if (!keep_testoutput && tmp_dir.size() > 0)
            remove_temporary_chaste_directory(tmp_dir);

        return ExecutableSupport::EXIT_ERROR;
    }
}

std::string temporary_chaste_directory()
{

    std::string testoutput_dir =
        OutputFileHandler::GetChasteTestOutputDirectory();

    std::string mkdir_cmd("mkdir -p ");
    mkdir_cmd += testoutput_dir;
    system(mkdir_cmd.c_str());

    std::string tmp_dir_template = std::string(testoutput_dir + "/tmp.XXXXXX");

    char *tmp_dir_path = new char[tmp_dir_template.size() + 1];
    strcpy(tmp_dir_path, tmp_dir_template.c_str());

    //std::cerr << "buffer: " << buffer << std::endl;

    if (mkdtemp(tmp_dir_path) == NULL)
    {
        perror("temporary_chaste_directory");
        exit(2);
    }

    std::string tmp_dir(basename(tmp_dir_path));

    delete[] tmp_dir_path;

    return tmp_dir;
}

void remove_temporary_chaste_directory(const std::string& tmp_dir)
{

        std::string testoutput_dir =
            OutputFileHandler::GetChasteTestOutputDirectory();

        std::string remove_cmd("rm -rf ");
        remove_cmd += testoutput_dir;
        remove_cmd += "/";
        remove_cmd += tmp_dir;

        system(remove_cmd.c_str());
}

int read_data_file(const std::string& data_file)
{

    std::fstream data_stream(data_file, std::fstream::in);

    int num_cells;
    data_stream >> num_cells;

    if (!data_stream.good())
    {
        std::string error_msg("could not read ");
        error_msg += data_file;
        std::runtime_error e(error_msg);
        throw e;
    }

    return num_cells;
}

void setup_singletons()
{

    // Set up what the test suite would do
    SimulationTime::Instance()->SetStartTime(0.0);

    CellCycleTimesGenerator::Instance()->SetRandomSeed( time(NULL) );

    RandomNumberGenerator::Instance()->Reseed( time(NULL) );

    CellPropertyRegistry::Instance()->Clear();
    CellId::ResetMaxCellId();
}

void destroy_singletons()
{
    SimulationTime::Destroy();
    CellCycleTimesGenerator::Destroy();
    RandomNumberGenerator::Destroy();
}
