#ifndef VERTEXBASEDMONOLAYER_HPP_
#define VERTEXBASEDMONOLAYER_HPP_

#include <string>

#include "Exception.hpp"

int VertexBasedMonolayerSimulation(
        const std::string& output_directory,
        double average_cell_cycle_time,
        double dt = DOUBLE_UNSET);

#endif // VERTEXBASEDMONOLAYER_HPP_
