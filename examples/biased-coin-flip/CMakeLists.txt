# Copy Python scripts
file (COPY prior-sampler.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY simulator.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturber.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY prior-pdf.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturbation-pdf.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy C++ code
file (COPY prior-sampler.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY simulator.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturber.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY prior-pdf.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY perturbation-pdf.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy data file
file (COPY heads.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell scripts
configure_script (run-python-abc-rejection-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-python-abc-rejection-serial-example.sh)

configure_script (run-python-abc-smc-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-python-abc-smc-serial-example.sh)

configure_script (run-cpp-abc-rejection-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-cpp-abc-rejection-serial-example.sh)

configure_script (run-cpp-abc-smc-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-cpp-abc-smc-serial-example.sh)

configure_script (run-python-abc-rejection-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-python-abc-rejection-mpi-example.sh)

configure_script (run-python-abc-smc-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-python-abc-smc-mpi-example.sh)

configure_script (run-cpp-abc-rejection-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-cpp-abc-rejection-mpi-example.sh)

configure_script (run-cpp-abc-smc-mpi-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-cpp-abc-smc-mpi-example.sh)

# Configure Python script
configure_script (plot-histogram.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/plot-histogram.py)

# Add executables
add_executable (coin-prior-sampler prior-sampler.cc)
add_executable (coin-simulator simulator.cc)
add_executable (coin-prior-pdf prior-pdf.cc)
add_executable (coin-perturber perturber.cc)
add_executable (coin-perturbation-pdf perturbation-pdf.cc)
