#include <string>
#include <vector>
#include <iostream>

#include <math.h>

/** Return probabilility density function of normal distribution.
 *
 * @param mu  mean of normal distribution.
 * @param sigma  standard deviation of normal distribution.
 * @param x  position to evaluate distribution on.
 *
 * @return probability density of normal distribution at x.
 */
double normal_pdf(double mu, double sigma, double x)
{

    return exp( - (x - mu) * (x - mu) / ( 2.0 * sigma * sigma ) ) /
                 sqrt( 2.0 * M_PI * sigma * sigma );
}

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " STDEV\n"
            "Return probability densities of normal distributions centered \n"
            "on given parameters for the perturbed parameter \n"
            "and with standard deviation STDEV\n";
        return 1;
    }

    double stdev = std::stod(argv[1]);

    // Read generation t and parameter from stdin
    int t;
    std::cin >> t;

    double q_perturbed;
    std::cin >> q_perturbed;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read t or q from stdin\n";
        return 1;
    }

    // Return normal_pdf for every parameter
    std::cout.precision(17);
    std::vector<double> q_vec;
    double q;
    for (std::cin >> q; !std::cin.eof(); std::cin >> q)
    {
        if (std::cin.fail())
        {
            std::cerr << "Error: could not read parameter from stdin\n";
            return 1;
        }

        q_vec.push_back(q);
    }

    for (const double& q : q_vec)
    {
        double pdf = normal_pdf(q, stdev, q_perturbed);
        std::cout << pdf << std::endl;
    }

    return 0;
}
