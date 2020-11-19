#include <string>
#include <iostream>
#include <chrono>
#include <random>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " Q_LOW Q_HIGH\n"
            "Sample from a uniform distribution with lower bound\n"
            "Q_LOW and upper bound Q_HIGH\n";
        return 1;
    }

    double q_low = std::stod(argv[1]);
    double q_high = std::stod(argv[2]);

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Sample from uniform distribution
    std::uniform_real_distribution<double> distribution(q_low, q_high);
    double q_sampled = distribution(generator);

    // Print sampled parameter
    std::cout.precision(17);
    std::cout << q_sampled << std::endl;

    return 0;
}
