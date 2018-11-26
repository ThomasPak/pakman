#include <iostream>
#include <stdexcept>

double uniform_pdf(const double a, const double b, const double x) {

    return ( (a <= x) && (x <= b) ) ? 1.0 / (b - a) : 0.0;
}

int main(int argc, char *argv[]) {

    using namespace std;

    // Process arguments
    if (argc != 1) {
        cerr << "Usage: " << argv[0] << endl;
        cerr << "Accepts \"a b x\" as stdin, "
                "where a is the lower bound, "
                "b is upper bound, "
                "and x is the argument" << endl;
        return 2;
    }

    // Process standard input
    double a, b;

    cin >> a >> b;

    if (cin.fail()) {
        runtime_error e("could not read or interpret standard input");
        throw e;
    }

    if (b <= a) {
        runtime_error e("a should be strictly less than b");
        throw e;
    }

    // Compute pdf inside interval
    double pdf = 1.0 / (b - a);

    // Print results
    cout.precision(17);
    double x;
    for (cin >> x; !cin.eof(); cin >> x) {

        if (cin.fail()) {
            runtime_error e("could not read or interpret standard input");
            throw e;
        }

        // Print result
        cout << uniform_pdf(a, b, x) << endl;
    }

    return 0;
}
