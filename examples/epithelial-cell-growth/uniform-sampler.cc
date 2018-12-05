#include <random>
#include <iostream>
#include <sstream>
#include <chrono>

int main(int argc, char *argv[]) {

    using namespace std;

    // Process arguments
    if ( (argc == 1 ) || (argc % 2 != 1) ) {
        cerr << "Usage: " << argv[0] << " MIN_VAL_1 MAX_VAL_1 .. MIN_VAL_N MAX_VAL_N"
             << endl;
        return 2;
    }

    // Seed RNG
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    // Print uniformly distributed random numbers
    ostringstream strm;
    strm.precision(17);

    for (int i = 1; i < argc; i = i + 2) {

        double a = stod(argv[i]);
        double b = stod(argv[i + 1]);

        uniform_real_distribution<double> distribution(a, b);

        strm << distribution(generator) << ' ';
    }

    string str(strm.str());
    str.pop_back();
    cout << str;

    return 0;
}
