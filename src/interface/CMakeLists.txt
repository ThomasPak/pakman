add_library (interface
    LineString.cc
    input.cc
    protocols.cc
    output.cc
    )

target_link_libraries (interface core)
