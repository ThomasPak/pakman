#include <iostream>
#include <random>
#include <chrono>
#include <stdexcept>

int main(int argc, char *argv[]) {

    using namespace std;

    // Process arguments
    if (argc != 1) {
        cerr << "Usage: " << argv[0] << endl;
        cerr << "Accepts \"p N\" as stdin, "
                "where p is the probability of success "
                "and N is the number of trials" << endl;
        return 2;
    }

    // Process standard input
    double p;
    int N;

    cin >> p >> N;

    if (cin.fail()) {
        runtime_error e("could not read or interpret standard input");
        throw e;
    }

    // Create binomial distribution and seed RNG
    binomial_distribution<int> distribution(N, p);

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    // Print answer
    cout << distribution(generator);

    return 0;
}
