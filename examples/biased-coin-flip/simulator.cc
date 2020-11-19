#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " DATAFILE\n"
            "Read number of heads from DATAFILE, "
            "epsilon and parameter from stdin,\n"
            "run simulation and output whether "
            "parameter is accepted or rejected\n";

        return 1;
    }

    std::fstream datafile(argv[1], std::fstream::in);

    if (datafile.fail())
    {
        std::cerr << "Error: could not open " << argv[1] << std::endl;
        return 1;
    }

    // Read epsilon and q from stdin
    int epsilon;
    double q;

    std::cin >> epsilon;
    std::cin >> q;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read epsilon or q from stdin\n";
        return 1;
    }

    // Read datafile into number of heads
    int obs_num_heads;
    datafile >> obs_num_heads;
    datafile.close();

    if (datafile.fail())
    {
        std::cerr << "Error: could not read data from " << argv[1]
            << std::endl;
        return 1;
    }

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Run simulation
    std::uniform_real_distribution<double> distribution;
    int sim_num_heads = 0;

    for (int i = 0; i < 20; i++)
    {
        if (distribution(generator) < q)
        {
            sim_num_heads++;
        }
    }

    // If absolute difference is less or equal than epsilon, accept, else
    // reject
    if (abs(sim_num_heads - obs_num_heads) <= epsilon)
    {
        std::cout << "accept\n";
    }
    else
    {
        std::cout << "reject\n";
    }

    return 0;
}
