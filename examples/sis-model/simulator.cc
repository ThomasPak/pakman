#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include <math.h>
#include <assert.h>

std::mt19937_64 generator;

/** Run Gillespie algorithm for SIS system and return time series of S.
 *
 *  A thorough explanation of the SIS model can be found at
 *  https://github.com/ThomasPak/pakman/wiki/Example:-SIS-model.
 *
 * @param beta  contact rate parameter.
 * @param gamma  recovery rate parameter.
 * @param S0  initial number of susceptible people.
 * @param I0  initial number of infected people.
 * @param tend  end of simulation time.
 * @param Nobs  number of observations to output.
 *
 * @return array containing simulated S counts at times
 *      t = 0, t = dt, t = 2 * dt, ... t = tend, where dt = tend / Nobs.
 */
std::vector<int> run_SIS_simulation(double beta, double gamma, int S0, int I0,
        double tend, int Nobs)
{
    // Initialize arrays
    std::vector<double> t(1, 0.0);
    std::vector<int> S(1, S0);
    std::vector<int> I(1, I0);

    // Initialize uniform real distribution
    std::uniform_real_distribution<double> distribution;

    // Apply Gillespie algorithm
    while (t.back() <= tend)
    {
        // If I == 0, the total propensity will be zero and no more reactions
        // can occur
        if (I.back() == 0)
            break;
        
        // Sample two uniformly distributed variables
        double r1 = distribution(generator);
        double r2 = distribution(generator);

        // Compute propensities
        double a1 = beta * S.back() * I.back();
        double a2 = gamma * I.back();
        double a0 = a1 + a2;

        // Compute waiting time
        double twait = 1.0 / a0 * log(1.0 / r1);
        t.push_back(t.back() + twait);

        // Choose reaction
        if (r2 * a0 < a1)
        // S + I --> 2I
        {
            S.push_back(S.back() - 1);
            I.push_back(I.back() + 1);
        }
        else
        // I --> S
        {
            S.push_back(S.back() + 1);
            I.push_back(I.back() - 1);
        }
    }

    // Extract S at desired time points
    std::vector<int> S_sim;
    double dt = tend / Nobs;
    double tnow = dt;

    auto S_it = S.cbegin();
    auto t_it = t.cbegin();

    while (S_sim.size() < Nobs)
    {
        // If end of array has been reached, break out of loop
        if ((t_it + 1) == t.cend())
            break;

        // Check if current time point is between *t_it and *(t_it + 1)
        if ((*t_it < tnow) && (tnow <= *(t_it + 1)))
        {
            S_sim.push_back(*S_it);
            tnow += dt;
        }
        else
        {
            t_it += 1;
            S_it += 1;
        }
    }

    // Fill in remaining entries
    S_sim.insert(S_sim.end(), Nobs - S_sim.size(), S.back());

    return S_sim;
}

double distance(std::vector<int> S_obs, std::vector<int> S_sim)
{
    assert(S_obs.size() == S_sim.size());

    // Compute sum of squared errors
    double sum = 0;
    for (int i = 0; i < S_obs.size(); i++)
        sum += (S_obs[i] - S_sim[i]) * (S_obs[i] - S_sim[i]);

    // Return square root of sum
    return sqrt(sum);
}

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 6)
    {
        std::cerr << "Usage: " << argv[0] << " S0 I0 TEND NOBS DATAFILE\n"
            "Read S and I counts from DATAFILE, "
            "epsilon and parameters beta and gamma from stdin,\n"
            "run simulation and output whether "
            "parameter is accepted or rejected\n";

        return 1;
    }

    int S0 = std::stoi(argv[1]);
    int I0 = std::stoi(argv[2]);
    double tend = std::stod(argv[3]);
    int Nobs = std::stoi(argv[4]);

    std::fstream datafile(argv[5], std::fstream::in);

    if (datafile.fail())
    {
        std::cerr << "Error: could not open " << argv[4] << std::endl;
        return 1;
    }

    // Read epsilon, beta and gamma from stdin
    double epsilon;
    double beta, gamma;

    std::cin >> epsilon;
    std::cin >> beta >> gamma;

    if (std::cin.fail())
    {
        std::cerr <<
            "Error: could not read epsilon, beta or gamma from stdin\n";
        return 1;
    }

    // Read datafile into array of S counts
    std::vector<int> S_obs(Nobs);
    for (int i = 0; i < Nobs; i++)
        datafile >> S_obs[i];
    datafile.close();

    if (datafile.fail())
    {
        std::cerr << "Error: could not read data from " << argv[4]
            << std::endl;
        return 1;
    }

    // Seed random number generator
    unsigned seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);

    // Run simulation
    std::vector<int> S_sim = run_SIS_simulation(beta, gamma, S0, I0, tend,
            Nobs);

    // Compute distance
    double dist = distance(S_obs, S_sim);

    // Print 'accept' if dist <= epsilon, else print 'reject'
    if (dist <= epsilon)
    {
        std::cout << "accept\n";
    }
    else
    {
        std::cout << "reject\n";
    }

    return 0;
}
