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

# Configure shell scripts, output to temporary directory and copy with
# appropriate permissions
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-scaling.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-scaling.sh"
    )

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/scaling-simulator.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/scaling-simulator.sh"
    )

file (COPY
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-scaling.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

file (COPY
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/scaling-simulator.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Remove temporary directory
file (REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/tmp")
