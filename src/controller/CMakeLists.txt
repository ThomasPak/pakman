include_directories (${PROJECT_SOURCE_DIR}/src)
include_directories (${PROJECT_SOURCE_DIR}/src/3rdparty)

add_library (controller
    AbstractController.cc
    AbstractControllerStatic.cc
    SweepController.cc
    SweepControllerStatic.cc
    ABCRejectionController.cc
    ABCRejectionControllerStatic.cc
    ABCSMCController.cc
    ABCSMCControllerStatic.cc
    Sampler.cc
    smc_weight.cc
    sample_population.cc
    )

target_link_libraries (controller core interface)
