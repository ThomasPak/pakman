#include <random>
#include <iostream>
#include <chrono>
#include <stdexcept>

int main(int argc, char *argv[]) {

    using namespace std;

    // Process arguments
    if (argc > 3) {
        cerr << "Usage: " << argv[0]
             << " [MIN_VAL] [MAX_VAL]"
             << endl;
        cerr << "Accepts \"mu sigma\" as stdin, "
                "where mu is the mean "
                "and sigma is the standard deviation" << endl;
        return 2;
    }

    double min_val, max_val;

    switch (argc) {
        case 1:
            min_val = - numeric_limits<double>::infinity();
            max_val = numeric_limits<double>::infinity();
            break;
        case 2:
            min_val = stod(argv[1]);
            max_val = numeric_limits<double>::infinity();
            break;
        case 3:
            min_val = stod(argv[1]);
            max_val = stod(argv[2]);
            break;
    }

    // Process standard input
    double mu, sigma;

    cin >> mu >> sigma;

    if (cin.fail()) {
        runtime_error e("could not read or interpret standard input");
        throw e;
    }

    // Seed RNG
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    // Print randomly distributed random number
    cout.precision(17);
    normal_distribution<double> distribution(mu, sigma);

    double result = distribution(generator);
    result = result > max_val ? max_val : result;
    result = result < min_val ? min_val : result;

    cout << result << endl;

    return 0;
}
