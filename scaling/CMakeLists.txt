# Add cpu-stress
add_executable (cpu-stress cpu-stress.c)
target_link_libraries (cpu-stress m)

# Get processor count
include (ProcessorCount)
ProcessorCount(cpu_count)

if (cpu_count EQUAL 0)
    message (WARNING "Could not obtain cpu count using ProcessorCount, "
        "using MPIEXEC_MAX_NUMPROCS instead")
    set (cpu_count ${MPIEXEC_MAX_NUMPROCS})
endif ()

# Copy Python script
file (COPY plot-scaling.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/scaling.rej.in"
    "${CMAKE_CURRENT_BINARY_DIR}/scaling.rej"
    )

# Configure shell scripts
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-scaling.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run-scaling.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/scaling-simulator.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/scaling-simulator.sh"
    )
