add_library (interface
    protocols.cc
    read_input.cc
    write_parameters.cc
    )

target_link_libraries (interface core)
