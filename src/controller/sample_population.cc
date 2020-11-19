#include <vector>
#include <random>
#include <stdexcept>

#include "sample_population.h"

void cumsum(const std::vector<double>& array,
        std::vector<double>& cumsum_array)
{

    double sum = 0.0;

    for (int i = 0; i < array.size(); i++)
        cumsum_array[i] = (sum += array[i]);
}

void normalize(std::vector<double>& array)
{

    double sum = 0.0;

    for (auto it = array.begin(); it != array.end(); it++)
        sum += *it;

    for (auto it = array.begin(); it != array.end(); it++)
        *it /= sum;
}

int sample_population(const std::vector<double>& norm_cumsum_array,
           std::uniform_real_distribution<double>& distribution,
           std::mt19937_64& generator)
{

    double u = distribution(generator);

    for (int idx = 0; idx < norm_cumsum_array.size(); idx++)
        if (u <= norm_cumsum_array[idx])
            return idx;

    // If execution reaches this, something must have gone wrong
    std::runtime_error e("could not sample population");
    throw e;
}
