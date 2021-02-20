add_library (controller
    AbstractController.cc
    AbstractControllerStatic.cc
    SweepController.cc
    SweepControllerStatic.cc
    ABCRejectionController.cc
    ABCRejectionControllerStatic.cc
    ABCSMCController.cc
    ABCSMCControllerStatic.cc
    AdaptiveABCSMCController.cc
    AdaptiveABCSMCControllerStatic.cc
    smc_weight.cc
    sample_population.cc
    )

target_link_libraries (controller core system interface master)
