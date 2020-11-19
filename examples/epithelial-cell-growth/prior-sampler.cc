#include <string>
#include <iostream>
#include <chrono>
#include <random>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " TCYCLE_LOW TCYCLE_HIGH\n"
            "Sample from a uniform distribution with lower bound\n"
            "TCYCLE_LOW and upper bound TCYCLE_HIGH\n";
        return 1;
    }

    double tcycle_low = std::stod(argv[1]);
    double tcycle_high = std::stod(argv[2]);

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Sample from uniform distribution
    std::uniform_real_distribution<double> distribution(tcycle_low,
            tcycle_high);
    double tcycle_sampled = distribution(generator);

    // Print sampled parameter
    std::cout.precision(17);
    std::cout << tcycle_sampled << std::endl;

    return 0;
}
