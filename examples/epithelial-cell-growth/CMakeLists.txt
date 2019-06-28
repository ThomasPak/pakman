# Check for CHASTE_DIR
if (NOT DEFINED CHASTE_DIR AND NOT DEFINED ENV{CHASTE_DIR})
    message (WARNING "CHASTE_DIR was not defined as environment or CMake variable, "
        "cannot build epithelial cell growth example.")
    return ()
endif ()

# If CMake variable CHASTE_DIR is defined, set it to environment variable
if (NOT DEFINED CHASTE_DIR)
    set (CHASTE_DIR "$ENV{CHASTE_DIR}")
endif ()

# Check if $CHASTE_DIR/projects/chaste-project already exists
if (EXISTS "${CHASTE_DIR}/projects/chaste-project")
    message (WARNING "The directory ${CHASTE_DIR}/projects/chaste-project already exists, "
        "building Pakman will overwrite this directory.")
else ()
    # Copy chaste-project to $CHASTE_DIR/projects
    file (COPY "chaste-project" DESTINATION "${CHASTE_DIR}/projects")
endif ()

# Check for CHASTE_BUILD_DIR
if (NOT DEFINED CHASTE_BUILD_DIR AND NOT DEFINED ENV{CHASTE_BUILD_DIR})
    file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/chaste-build")
    set (CHASTE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/chaste-build")
elseif (NOT DEFINED CHASTE_BUILD_DIR)
    set (CHASTE_BUILD_DIR "$ENV{CHASTE_BUILD_DIR}")
endif ()

# Add custom target to create chaste simulator
add_custom_target (chaste-test ALL DEPENDS cell-simulator)
add_custom_command (OUTPUT cell-simulator
    COMMAND echo "Building Chaste simulator"
    COMMAND echo '************************************'
    COMMAND rm -rf "${CHASTE_DIR}/projects/chaste-project"
    COMMAND ln -s "${CMAKE_CURRENT_SOURCE_DIR}/chaste-project" "${CHASTE_DIR}/projects"
    COMMAND cmake -B"${CHASTE_BUILD_DIR}" -H"${CHASTE_DIR}"
    COMMAND cmake --build "${CHASTE_BUILD_DIR}" --target
    VertexBasedMonolayerSimulator -- ${CHASTE_BUILD_OPTIONS}
    COMMAND cp
    "${CHASTE_BUILD_DIR}/projects/chaste-project/apps/VertexBasedMonolayerSimulator"
    cell-simulator
    COMMAND echo '************************************'
    COMMAND echo "Done building Chaste simulator"
    MAIN_DEPENDENCY chaste-project/apps/src/VertexBasedMonolayerSimulator.cpp
    DEPENDS chaste-project/src/VertexBasedMonolayerSimulation.cpp
    chaste-project/src/VertexBasedMonolayerSimulation.hpp
    "${PROJECT_SOURCE_DIR}/include/pakman_mpi_worker.h"
    )

# Copy project
file (COPY chaste-project
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy Python scripts

# Copy C++ code
file (COPY prior-sampler.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturber.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY prior-pdf.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturbation-pdf.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy data file
file (COPY cell_count.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell scripts
configure_script (run-abc-rejection-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-rejection-serial-example.sh)

configure_script (run-abc-smc-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-smc-serial-example.sh)

# Configure Python script
#configure_script (plot-histogram.py.in
#    ${CMAKE_CURRENT_BINARY_DIR}/plot-histogram.py)

# Add executables
add_executable (cell-prior-sampler prior-sampler.cc)
add_executable (cell-prior-pdf prior-pdf.cc)
add_executable (cell-perturber perturber.cc)
add_executable (cell-perturbation-pdf perturbation-pdf.cc)
