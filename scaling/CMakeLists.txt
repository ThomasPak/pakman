# Add heat-equation
add_executable (heat-equation heat-equation.cc)

# Get processor count
include (ProcessorCount)
ProcessorCount(cpu_count)

if (cpu_count EQUAL 0)
    message (WARNING "Could not obtain cpu count using ProcessorCount, "
        "using MPIEXEC_MAX_NUMPROCS instead")
    set (cpu_count ${MPIEXEC_MAX_NUMPROCS})
endif ()

# Configure Python script
configure_script (plot-scaling.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/plot-scaling.py)

# Configure shell scripts
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-scaling.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run-scaling.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/scaling-simulator.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/scaling-simulator.sh"
    )
