#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include <math.h>

// Define normal pdf
double normal_pdf(double mu, double sigma, double x)
{

    return exp( - (x - mu) * (x - mu) / ( 2.0 * sigma * sigma ) ) /
                 sqrt( 2.0 * M_PI * sigma * sigma );
}

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " STDEV1 STDEV2\n"
            "Return probability densities of "
            "joint normal distributions centered \n"
            "on given parameters for the perturbed parameter \n"
            "and with standard deviations STDEV1 and STDEV2\n";
        return 1;
    }

    double stdev1 = std::stod(argv[1]);
    double stdev2 = std::stod(argv[2]);

    // Read generation t and parameter from stdin
    int t;
    std::cin >> t;

    double beta_perturbed, gamma_perturbed;
    std::cin >> beta_perturbed >> gamma_perturbed;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read t, beta_perturbed "
            "or gamma_perturbed from stdin\n";
        return 1;
    }

    // Return normal_pdf for every parameter
    std::cout.precision(17);
    std::vector<std::pair<double,double>> bg_vec;
    double beta, gamma;
    for (std::cin >> beta >> gamma; !std::cin.eof(); std::cin >> beta >> gamma)
    {
        if (std::cin.fail())
        {
            std::cerr << "Error: could not read parameter from stdin\n";
            return 1;
        }

        bg_vec.emplace_back(beta, gamma);
    }

    for (const auto& bg : bg_vec)
    {
        double beta = bg.first;
        double gamma = bg.second;

        double pdf1 = normal_pdf(beta, stdev1, beta_perturbed);
        double pdf2 = normal_pdf(gamma, stdev2, gamma_perturbed);
        double pdf = pdf1 * pdf2;
        std::cout << pdf << std::endl;
    }

    return 0;
}
