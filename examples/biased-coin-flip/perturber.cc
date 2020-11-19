#include <string>
#include <iostream>
#include <random>
#include <chrono>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " STDEV\n"
            "Perturb given parameter by drawing "
            "from normal distribution\n"
            "centered on the given parameter "
            "and with standard deviation STDEV\n";
        return 1;
    }

    double stdev = std::stod(argv[1]);

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Read generation t and parameter from stdin
    int t;
    std::cin >> t;

    double q;
    std::cin >> q;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read t or q from stdin\n";
        return 1;
    }

    // Perturb parameter
    std::cout.precision(17);
    std::normal_distribution<double> distribution(q, stdev);
    double q_perturbed = distribution(generator);

    // Print perturbed parameter
    std::cout << q_perturbed << std::endl;

    return 0;
}
