# Copy Python scripts
file (COPY biased-coin-flip.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY create-histogram.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Configure shell script, output to temporary directory and copy with
# appropriate permissions
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/run-example.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-example.sh"
    )

file (COPY
    "${CMAKE_CURRENT_BINARY_DIR}/tmp/run-example.sh"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Remove temporary directory
file (REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/tmp")
