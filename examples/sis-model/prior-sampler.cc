#include <string>
#include <iostream>
#include <chrono>
#include <random>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0]
            << " BETA_LOW BETA_HIGH GAMMA_LOW GAMMA_HIGH\n"
            "Sample from two uniform distributions; one with lower bound\n"
            "BETA_LOW and upper bound BETA_HIGH, and one with lower bound\n"
            "GAMMA_LOW and upper bound GAMMA_HIGH.\n";
        return 1;
    }

    double beta_low = std::stod(argv[1]);
    double beta_high = std::stod(argv[2]);
    double gamma_low = std::stod(argv[3]);
    double gamma_high = std::stod(argv[4]);

    // Seed random number generator
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Sample from uniform distribution for beta
    std::uniform_real_distribution<double> beta_distribution(beta_low, beta_high);
    double beta_sampled = beta_distribution(generator);

    // Sample from uniform distribution for gamma
    std::uniform_real_distribution<double> gamma_distribution(gamma_low, gamma_high);
    double gamma_sampled = gamma_distribution(generator);

    // Print sampled parameters
    std::cout.precision(17);
    std::cout << beta_sampled << " " << gamma_sampled << std::endl;

    return 0;
}
