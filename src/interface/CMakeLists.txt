add_library (interface
    LineString.cc
    input.cc
    protocols.cc
    write_parameters.cc
    )

target_link_libraries (interface core)
