add_library (interface
    Parameter.cc
    protocols.cc
    write_parameters.cc
    )

target_link_libraries (interface core)
