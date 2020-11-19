#include <string>
#include <iostream>
#include <random>
#include <chrono>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " STDEV1 STDEV2\n"
            "Perturb two parameters by drawing "
            "from normal distributions\n"
            "centered on the given parameters "
            "and with standard deviations\n"
            "STDEV1 and STDEV2, respectively.\n";
        return 1;
    }

    double stdev1 = std::stod(argv[1]);
    double stdev2 = std::stod(argv[2]);

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 generator(seed);

    // Read generation t and parameter from stdin
    int t;
    std::cin >> t;

    double beta, gamma;
    std::cin >> beta >> gamma;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read t, beta or gamma from stdin\n";
        return 1;
    }

    // Perturb parameter
    std::cout.precision(17);
    std::normal_distribution<double> distribution1(beta, stdev1);
    std::normal_distribution<double> distribution2(gamma, stdev2);

    double beta_perturbed = distribution1(generator);
    double gamma_perturbed = distribution2(generator);

    // Print perturbed parameter
    std::cout << beta_perturbed << " " << gamma_perturbed << std::endl;

    return 0;
}
