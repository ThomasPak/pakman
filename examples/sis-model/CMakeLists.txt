# Copy Python scripts
file (COPY sis-analytical.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy C++ code
file (COPY simulator.cc
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy data file
file (COPY S_data.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell scripts
configure_script (run-abc-rejection-serial-example.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run-abc-rejection-serial-example.sh)

# Configure Python script
#configure_script (plot-histogram.py.in
#    ${CMAKE_CURRENT_BINARY_DIR}/plot-histogram.py)

# Add executables
add_executable (sis-simulator simulator.cc)
add_executable (sis-prior-sampler prior-sampler.cc)
add_executable (sis-prior-pdf prior-pdf.cc)
