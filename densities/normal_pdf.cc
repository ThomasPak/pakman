#include <iostream>
#include <cmath>
#include <stdexcept>

double normal_pdf(const double mu, const double sigma, const double x)
{

    return exp( - (x - mu) * (x - mu) / ( 2.0 * sigma * sigma ) ) /
                 sqrt( 2.0 * M_PI * sigma * sigma );
}


int main(int argc, char *argv[])
{

    using namespace std;

    // Process arguments
    if (argc != 1)
    {
        cerr << "Usage: " << argv[0] << endl;
        cerr << "Accepts \"mu sigma x\" as stdin, "
                "where mu is the mean, "
                "sigma is the standard deviation, "
                "and x is the argument" << endl;
        return 2;
    }

    // Process standard input
    double mu, sigma;

    cin >> mu >> sigma;

    if (cin.fail())
    {
        runtime_error e("could not read or interpret standard input");
        throw e;
    }

    if (sigma <= 0.0)
    {
        runtime_error e("sigma must be strictly positive");
        throw e;
    }

    // Compute pdf
    cout.precision(17);
    double x;
    for (cin >> x; !cin.eof(); cin >> x)
    {

        if (cin.fail())
        {
            runtime_error e("could not read or interpret standard input");
            throw e;
        }

        // Print result
        cout << normal_pdf(mu, sigma, x) << endl;
    }

    return 0;
}
