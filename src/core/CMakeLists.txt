include_directories (${PROJECT_SOURCE_DIR}/src)
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_library (core
    utils.cc
    Arguments.cc
    LongOptions.cc
    Parameter.cc
    timer.cc
    )
