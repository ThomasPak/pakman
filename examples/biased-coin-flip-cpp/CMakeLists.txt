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

# Configure shell scripts
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-example.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run-example.sh"
    )

configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/biased-coin-flip-simulator.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/biased-coin-flip-simulator.sh"
    )
