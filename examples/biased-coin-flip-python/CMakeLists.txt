# Copy Python scripts
file (COPY biased-coin-flip.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY create-histogram.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell script
configure_script (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-example.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/run-example.sh"
    )
