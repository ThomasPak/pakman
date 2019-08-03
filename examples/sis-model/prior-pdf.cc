#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] <<
            " BETA_LOW BETA_HIGH GAMMA_LOW GAMMA_HIGH\n"
            "Return probability density of prior distribution with\n"
            "uniformly distributed beta with lower bound "
            "BETA_LOW and upper BETA_HIGH,\n"
            "and uniformly distributed gamma with "
            "lower bound GAMMA_LOW and upper GAMMA_HIGH,\n"
            "for given parameter\n";
        return 1;
    }

    double beta_low = std::stod(argv[1]);
    double beta_high = std::stod(argv[2]);
    double gamma_low = std::stod(argv[3]);
    double gamma_high = std::stod(argv[4]);

    if (beta_high <= beta_low)
    {
        std::cerr << "Error: BETA_LOW must be strictly less than Q_HIGH\n";
        return 1;
    }

    if (gamma_high <= gamma_low)
    {
        std::cerr <<
            "Error: GAMMA_LOW must be strictly less than GAMMA_HIGH\n";
        return 1;
    }

    // Read parameter from stdin
    double beta, gamma;
    std::cin >> beta >> gamma;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read beta or gamma from stdin\n";
        return 1;
    }

    // Check if parameters are within bounds
    std::cout.precision(17);
    if ( (beta_low <= beta) && (beta <= beta_high) &&
         (gamma_low <= gamma) && (gamma <= gamma_high) )
    {
        std::cout << 1.0 / (beta_high - beta_low) / (gamma_high - gamma_low) <<
            std::endl;
    }
    else
    {
        std::cout << 0 << std::endl;
    }

    return 0;
}
