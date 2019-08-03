#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " TCYCLE_LOW TCYCLE_HIGH\n"
            "Return probability density of uniform distribution with \n"
            "lower bound TCYCLE_LOW and upper bound TCYCLE_HIGH "
            "for given parameter\n";
        return 1;
    }

    double tcycle_low = std::stod(argv[1]);
    double tcycle_high = std::stod(argv[2]);

    if (tcycle_high <= tcycle_low)
    {
        std::cerr <<
            "Error: TCYCLE_LOW must be strictly less than TCYCLE_HIGH\n";
        return 1;
    }

    // Read parameter from stdin
    double tcycle;
    std::cin >> tcycle;

    if (std::cin.fail())
    {
        std::cerr << "Error: could not read tcycle from stdin\n";
        return 1;
    }

    // Check if tcycle is within bounds
    std::cout.precision(17);
    if ( (tcycle_low <= tcycle) && (tcycle <= tcycle_high) )
    {
        std::cout << 1.0 / (tcycle_high - tcycle_low) << std::endl;
    }
    else
    {
        std::cout << 0 << std::endl;
    }

    return 0;
}
