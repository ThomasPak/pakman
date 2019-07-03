#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " Q_LOW Q_HIGH\n"
            "Return probability density of uniform distribution with \n"
            "lower bound Q_LOW and upper bound Q_HIGH "
            "for given parameter\n";
        return 1;
    }

    double q_low = std::stod(argv[1]);
    double q_high = std::stod(argv[2]);

    if (q_high <= q_low)
    {
        std::cerr << "Error: Q_LOW must be strictly less than Q_HIGH\n";
        return 1;
    }

    // Read parameter from stdin
    double q;
    std::cin >> q;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read q from stdin\n";
        return 1;
    }

    // Check if q is within bounds
    std::cout.precision(17);
    if ( (q_low <= q) && (q <= q_high) )
    {
        std::cout << 1.0 / (q_high - q_low) << std::endl;
    }
    else
    {
        std::cout << 0 << std::endl;
    }

    return 0;
}
