include_directories (${PROJECT_SOURCE_DIR}/src)
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_library (interface
    protocols.cc
    read_input.cc
    write_parameters.cc
    )
