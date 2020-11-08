# CHASTE_DIR was not set, return immediately
if (CHASTE_DIR STREQUAL "")
    return ()
endif ()

# Check if CHASTE_DIR is directory
if (NOT IS_DIRECTORY ${CHASTE_DIR})
    message (WARNING "CHASTE_DIR (\"${CHASTE_DIR}\") is not a valid directory, "
        "cannot build epithelial cell growth example.")
    return ()
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
if (NOT IS_DIRECTORY ${CHASTE_BUILD_DIR})
    message (WARNING "CHASTE_BUILD_DIR (\"${CHASTE_BUILD_DIR}\") is not a valid directory, "
        "building Chaste in the build directory ${CMAKE_CURRENT_BINARY_DIR}/chaste-build.")
    file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/chaste-build")
    set (CHASTE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/chaste-build")
endif ()

# Configure Chaste build
execute_process (COMMAND rm -rf "${CHASTE_DIR}/projects/chaste-project")
execute_process (COMMAND ln -s "${CMAKE_CURRENT_SOURCE_DIR}/chaste-project"
    "${CHASTE_DIR}/projects")
execute_process (COMMAND echo "Running CMake on Chaste")
execute_process (COMMAND echo "Chaste directory: ${CHASTE_DIR}")
execute_process (COMMAND echo "************************************")
execute_process (COMMAND cmake "-B${CHASTE_BUILD_DIR}" "-H${CHASTE_DIR}")
execute_process (COMMAND echo "************************************")
execute_process (COMMAND echo "Done running CMake on Chaste")

# Add custom target to create chaste standard simulator
add_custom_target (chaste-test ALL DEPENDS cell-standard-simulator cell-mpi-simulator)
add_custom_command (OUTPUT cell-standard-simulator cell-mpi-simulator
    COMMAND echo "Building Chaste simulators"
    COMMAND echo "Chaste build directory: ${CHASTE_BUILD_DIR}"
    COMMAND echo '************************************'
    COMMAND cmake --build "${CHASTE_BUILD_DIR}" --target
    VertexBasedMonolayerStandardSimulator -- ${CHASTE_BUILD_OPTIONS}
    COMMAND cmake --build "${CHASTE_BUILD_DIR}" --target
    VertexBasedMonolayerMPISimulator -- ${CHASTE_BUILD_OPTIONS}
    COMMAND cp
    "${CHASTE_BUILD_DIR}/projects/chaste-project/apps/VertexBasedMonolayerStandardSimulator"
    cell-standard-simulator
    COMMAND cp
    "${CHASTE_BUILD_DIR}/projects/chaste-project/apps/VertexBasedMonolayerMPISimulator"
    cell-mpi-simulator
    COMMAND echo '************************************'
    COMMAND echo "Done building Chaste simulators"
    MAIN_DEPENDENCY chaste-project/apps/src/VertexBasedMonolayerStandardSimulator.cpp
    DEPENDS chaste-project/apps/src/VertexBasedMonolayerMPISimulator.cpp
    chaste-project/src/VertexBasedMonolayerSimulation.cpp
    chaste-project/src/VertexBasedMonolayerSimulation.hpp
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
configure_script (run-cell-standard-simulator.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-cell-standard-simulator.sh)

configure_script (run-abc-rejection-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-rejection-serial-example.sh)

configure_script (run-abc-smc-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-smc-serial-example.sh)

configure_script (run-abc-rejection-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-rejection-mpi-example.sh)

configure_script (run-abc-smc-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-smc-mpi-example.sh)

# Configure Python script
configure_script (plot-histogram.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/plot-histogram.py)

# Add executables
add_executable (cell-prior-sampler prior-sampler.cc)
add_executable (cell-prior-pdf prior-pdf.cc)
add_executable (cell-perturber perturber.cc)
add_executable (cell-perturbation-pdf perturbation-pdf.cc)
