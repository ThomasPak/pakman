# Copy Python scripts
file (COPY stateful-prior-sampler.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY stateful-simulator.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY stateful-perturber.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY ${CMAKE_SOURCE_DIR}/examples/biased-coin-flip/prior-pdf.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY ${CMAKE_SOURCE_DIR}/examples/biased-coin-flip/perturbation-pdf.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy data file
file (COPY ${CMAKE_SOURCE_DIR}/examples/biased-coin-flip/heads.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell scripts
configure_script (test-stateful-python-abc-rejection-serial.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/test-stateful-python-abc-rejection-serial.sh)

configure_script (test-stateful-python-abc-smc-serial.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/test-stateful-python-abc-smc-serial.sh)

# Add tests
add_test (ABCRejectionSeed
    "${CMAKE_CURRENT_BINARY_DIR}/test-stateful-python-abc-rejection-serial.sh")

add_test (ABCSMCSeed
    "${CMAKE_CURRENT_BINARY_DIR}/test-stateful-python-abc-smc-serial.sh")
