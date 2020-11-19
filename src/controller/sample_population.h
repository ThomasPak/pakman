#ifndef SAMPLE_POPULATION_H
#define SAMPLE_POPULATION_H

#include <vector>
#include <random>

void cumsum(const std::vector<double>& array,
        std::vector<double>& cumsum_array);
void normalize(std::vector<double>& array);
int sample_population(const std::vector<double>& norm_cumsum_array,
           std::uniform_real_distribution<double>& distribution,
           std::mt19937_64& generator);

#endif // SAMPLE_POPULATION_H
