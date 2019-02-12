# Copy Python script
file (COPY "${PROJECT_SOURCE_DIR}/examples/biased-coin-flip-python/create-histogram.py"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add biased coin flip sampler
add_executable (biased-coin-flip-sampler biased-coin-flip-sampler.cc)

# Configure input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/biased-coin-flip.rej.in"
    "${CMAKE_CURRENT_BINARY_DIR}/biased-coin-flip.rej"
    )

# Configure shell scripts, output to temporary directory and copy with
# appropriate permissions
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-example.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-example.sh"
    )

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/biased-coin-flip-simulator.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/biased-coin-flip-simulator.sh"
    )

file (COPY
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-example.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

file (COPY
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/biased-coin-flip-simulator.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Remove temporary directory
file (REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/tmp")
