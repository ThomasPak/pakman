#ifdef DOXYGEN_ONLY

// This header contains no code.  It only exists to contain stand-alone Doxygen
// documentation pages.

/** @mainpage
 *
 * # Introduction           {#introduction}
 *
 * Welcome to the code documentation of Pakman!  If you stumbled here without
 * any context whatsoever, you should have a look at the [Pakman
 * repository](https://github.com/ThomasPak/pakman) first.  There, you can find
 * high-level instructions for installing and using Pakman.
 *
 * These pages contain documentation generated from the source files of the
 * Pakman project, as well as some advanced instructions that are not found in
 * the [Pakman wiki](https://github.com/ThomasPak/pakman/wiki) (e.g. how to
 * extend Pakman).  For an overview of the classes and files defined in the
 * Pakman project, see the sidebar on the left.
 *
 * # Organisation           {#organisation}
 *
 * The Pakman source files in **src** are organised into seven subdirectories:
 *
 * - **core** Common classes and functions.
 * - **system** Functions for handling system calls, e.g. forking, pipes, etc.
 * - **mpi** Functions for interacting with the MPI library.
 * - **master** Implements the Master class hierarchy (see [Software
 *   architecture](#software_architecture)).
 * - **controller** Implements the Controller class hierarchy (see [Software
 *   architecture](#software_architecture)).
 * - **interface** Functions that implement the interface between the user and
 *   Pakman, as well as between user executables and Pakman.
 * - **main** Implements the front-end of Pakman, including help messages (see
 *   help.h).
 *
 * In addition, the **3rdparty** subdirectory contains code imported from
 * external open-source projects.
 *
 * Lastly, the **include** directory contains headers for MPI simulators (see
 * @ref mpi-simulator "here" for more information on MPI simulators).
 *
 * # Software architecture  {#software_architecture}
 *
 * The software architecture of Pakman is based on two main classes: the
 * AbstractController and the AbstractMaster.  Briefly, the AbstractController
 * is responsible for issuing simulation tasks, while the AbstractMaster is
 * responsible for executing these simulation tasks (and this may happen in a
 * parallel manner).
 *
 * The interaction between Controllers and Masters are abstracted by two
 * queues; the Controller pushes pending tasks to **pending queue** of the
 * Master, and pops finished tasks from the front of the **finished queue**.
 * The role of the Master is therefore simply to process tasks from the pending
 * queue and push the finished tasks on to the finished queue.
 *
 * Moreover, Masters and Controllers are run within an **event loop**.  This
 * means that they both implement an `iterate()` function
 * (AbstractMaster::iterate() and AbstractController::iterate(), respectively),
 * which is called within a loop until the program terminates.  At every
 * iteration, the Master checks for finished simulations, schedules new
 * simulations and updates its task queues accordingly.  On the other hand, the
 * Controller receives the results of the finished simulations and schedules
 * new simulations based on the algorithm that it is implementing.  When the
 * algorithm has finished, the Controller writes out the appropriate output and
 * orders the Master to terminate.
 *
 * Therefore, extending Pakman with a new ABC algorithm is relatively
 * straightforward; you only need to create a new Controller subclass and
 * formulate the algorithm in an iterative manner using tasks queues.  It does
 * not require the user to fiddle with the machinery for parallelising
 * simulations.
 *
 * # Advanced instructions  {#advanced_instructions}
 *
 * If your simulator uses MPI, you are likely to run into troubles when trying
 * to use it directly with MPIMaster.  This is because an MPI program forking
 * another MPI program is not supported.  For this reason, we have implemented
 * a separate communication protocol for MPI simulators.  See [this wiki
 * page](
 * https://github.com/ThomasPak/pakman/wiki/Example:-epithelial-cell-growth)
 * for an example of an MPI simulator and see @ref mpi-simulator "this
 * documentation page" for instructions on how to implement an MPI simulator.
 *
 * If you wish to implement an ABC algorithm that is currently not part of
 * Pakman, please see @ref controller "this link" on how to write a new
 * Controller subclass.
 */

/** @page mpi-simulator Implementing an MPI simulator
 *
 * # MPI simulators
 *
 * As mentioned in @ref index "the main page", it is likely that you will run
 * into problems when your simulator uses MPI and you try to use it with
 * MPIMaster directly.  This error occurs because an MPI application forking
 * another MPI application is not defined by the [MPI
 * standard](https://www.mpi-forum.org/docs/) and is not supported by current
 * MPI libraries (and likely never will be).
 *
 * Fortunately, there exists a mechanism native to the MPI standard for the
 * creation of new processes.  In particular, the MPI function `MPI_Comm_spawn`
 * allows you to spawn a new MPI process and provides an MPI communicator to
 * communicate with the child MPI process after it has been created.
 *
 * However, in order to maintain portability, spawned MPI processes have
 * certain limitations when it comes to process control.  Firstly, there is no
 * way to force the termination of a process spawned using `MPI_Comm_spawn`.
 * When using a "standard" (i.e.\ a forked) simulator, Pakman can send system
 * signals to terminate Workers before they have finished their simulations,
 * for example when an ABC SMC generation has finished, or the requisite number
 * of parameters have been accepted in ABC rejection.  This is not possible for
 * spawned MPI processes however, so Pakman has no choice but to wait for the
 * simulation to finish.
 *
 * Secondly, it is impossible to discard the standard error of a process
 * created using `MPI_Comm_spawn`, so the flag `--discard-child-stderr` does
 * not have any effect.  Thirdly, spawned MPI programs should not be wrapped in
 * a shell script because this is not defined by the MPI standard (even though
 * some MPI libraries may support this).
 *
 * Most importantly, using an MPI-based simulator with Pakman breaks Pakman's
 * modular framework because now the communication between Pakman and the
 * simulator has to happen through MPI instead of through standard input and
 * output.  The simulator is thus no longer a black box on the systems-level,
 * but rather has to be implemented as C or C++ function.  As a result, the
 * protocol for communicating between Pakman and the simulator has to be
 * compiled into the user executable.
 *
 * We have implemented the Pakman--Worker communication protocol as a C header
 * and as a C++ header in pakman_mpi_worker.h and PakmanMPIWorker.hpp,
 * respectively.  These headers define a **Pakman MPI Worker**.  The two roles
 * of a Pakman MPI Worker are to communicate with Pakman and to run
 * user-defined simulations.
 *
 * @note When using Pakman with an MPI simulator, you must use the flag
 * `--mpi-simulator` so that Pakman is aware of this.  See [this wiki
 * page](
 * https://github.com/ThomasPak/pakman/wiki/Example:-epithelial-cell-growth)
 * for an example.
 *
 * # C: pakman_mpi_worker.h
 *
 * When writing an MPI simulator in C, the user needs to define a function with
 * the following signature:
 *
 * ```C
 * int simulator(int argc, char *argv[],
 *             const char *input_string, char **p_output_string);
 * ```
 *
 * This function should perform the same tasks that a standard simulator user
 * executable would do; given some command-line arguments and an input_string
 * (which contains a tolerance and a parameter), run a simulation, compare the
 * simulated data to the observed data and return an output string (containing
 * either `accept` or `reject`).  Moreover, the return value is considered the
 * "exit code" of the simulation; a nonzero return value indicates that an
 * error has occurred during the simulation.
 *
 * After defining this function, it should be passed on as a function pointer
 * to the function pakman_run_mpi_worker(), defined in pakman_mpi_worker.h with
 * the signature
 *
 * ```C
 * int pakman_run_mpi_worker(
 *         int argc, char *argv[],
 *         int (*simulator)(int argc, char *argv[],
 *             const char *input_string, char **p_output_string));
 * ```
 *
 * Hence the template for an MPI simulator written in C is as follows:
 *
 *
 * ```C
 * #include "pakman_mpi_worker.h"
 *
 * int my_simulator(int argc, char *argv[],
 *         const char* input_string, char **p_output_string)
 * {
 *     /* ...
 *      * Perform simulation
 *      * ...
 *      */
 * }
 *
 * int main(int argc, char *argv[])
 * {
 *     /* Initialize MPI */
 *     MPI_Init(NULL, NULL);
 *
 *     /* Run MPI Worker */
 *     pakman_run_mpi_worker(argc, argv, &my_simulator);
 *
 *     /* Finalize MPI */
 *     MPI_Finalize();
 *
 *     return 0;
 * }
 * ```
 *
 * Note that the user still needs to call `MPI_Init` and `MPI_Finalize`,
 * otherwise the MPI Worker will attempt to call MPI functions without
 * initializing MPI.
 *
 * When Pakman is invoked with an MPI simulator, it will spawn the MPI Worker
 * once and repeatedly execute the simulator function to run simulations.  When
 * Pakman terminates, it will send a termination signal to the MPI Worker
 * through MPI.  Only then will MPI Worker terminate.  This is in contrast to
 * the standard Worker, which is forked from Pakman for each simulation.
 *
 * ## Example
 *
 * The following MPI simulator example can be found is taken from
 * mpi-simulator.c.  It is a dummy simulator that by default simply outputs
 * `accept` and exits with a zero error code.  In addition, the output message
 * and the error code can be specified with optional command-line arguments.
 *
 * @include mpi-simulator.c
 *
 * # C++: PakmanMPIWorker.hpp
 *
 * When writing an MPI simulator in C++, the user can use pakman_mpi_worker.h
 * as before.  However, it is also possible to use the PakmanMPIWorker class,
 * defined in PakmanMPIWorker.hpp.  This method has the advantage that it is
 * not constrained to function pointers, but rather accepts instantiations of
 * the
 * [`std::function`](http://www.cplusplus.com/reference/functional/function/)
 * class.  This is a class that can wrap any callable element, including
 * function pointers.  In our case, the expected `std::function` object is of
 * the following type:
 *
 * ```
 * std::function<int(int argc, char** argv, const std::string& input_string,
 *     std::string& output_string)>
 * ```
 *
 * The arguments and return value retain the same meaning as in
 * pakman_mpi_worker.h.
 *
 * This function object should be passed to the constructor of PakmanMPIWorker.
 * Then, to run the MPI Worker, the user needs to call PakmanMPIWorker::run()
 * on the created object.
 *
 * Assuming that the simulator function is written as a normal function, the
 * template of an MPI simulator written with PakmanMPIWorker.hpp is as follows:
 *
 * ```
 * #include "PakmanMPIWorker.hpp"
 *
 * int my_simulator(int argc, char *argv[],
 *         const std::string& input_string, std::string& output_string)
 * {
 *     // ...
 *     // Perform simulation
 *     // ...
 * }
 *
 * int main(int argc, char *argv[])
 * {
 *     // Initialize MPI
 *     MPI_Init(nullptr, nullptr);
 *
 *     // Create MPI Worker
 *     PakmanMPIWorker worker(&my_simulator);
 *
 *     // Run MPI Worker
 *     worker.run(argc, argv);
 *
 *     // Finalize MPI
 *     MPI_Finalize();
 *
 *     return 0;
 * }
 * ```
 *
 * As before, the user still needs to call `MPI_Init` and `MPI_Finalize`.
 *
 * ## Example
 *
 * The following MPI simulator example can be found is taken from
 * mpi-simulator-cpp.cc.  It is a dummy simulator that by default simply
 * outputs `accept` and exits with a zero error code.  Furthermore, the output
 * message and the error code can be specified with optional command-line
 * arguments.
 *
 * @include mpi-simulator-cpp.cc
 */

/** @page controller Implementing a Controller subclass
 *
 * In order to implement an ABC algorithm in Pakman, you need to formulate the
 * algorithm in an iterative manner in a Controller class.  This means that the
 * same `iterate()` function is called over and over, and at every iteration
 * you have a set of actions that you can take.  You can push new tasks to the
 * Master, receive finished tasks, flush the Master's tasks queues, or
 * terminate the Master, as well as any combination of these actions (although
 * you can only terminate the Master once, as the program will then terminate).
 *
 * The Master can be accessed by the inherited member variable
 * AbstractController::m_p_master.  For example, terminating the Master is done
 * by calling `m_p_master->terminate()`.  See the documentation of
 * AbstractMaster for a full list of public member functions that you can use
 * in a Controller class.
 *
 * The classes ABCRejectionController, ABCSMCController, and SweepController
 * provide examples of how to implement the ABC rejection, the ABC SMC, and the
 * parameter sweep algorithms iteratively.
 *
 * In order to integrate a new Controller class called `ExampleController` into
 * Pakman, you need to follow these steps:
 *
 * 1. Add the `example` Controller type to the @ref controller_t enum type,
 * defined in common.h.
 *
 * 2. Add an `else if` statement to AbstractController::getController() in
 * AbstractControllerStatic.cc to translate the appropriate command-line
 * argument into the right @ref controller_t value.
 *
 * 3. Write a new Controller class in `src/controller/ExampleController.h` that
 * inherits from AbstractController.
 *
 * 4. Override the pure virtual functions AbstractController::iterate() and
 * AbstractController::getSimulator() in `src/controller/ExampleController.cc`.
 *
 * 5. Implement the static functions `%help()`, `addLongOptions()`, and
 * `makeController()` in `src/controller/ExampleControllerStatic.cc`.
 *
 * 6. Add corresponding entries to the `switch` statements in
 * AbstractController::help(), AbstractController::addLongOptions(), and
 * AbstractController::makeController() in AbstractControllerStatic.cc.
 *
 * 7. Add a line to the Pakman overview message generated by overview() in
 * help.cc.
 *
 * 8. Add source files `ExampleController.cc` and `ExampleControllerStatic.cc`
 * to `src/controller/CMakeLists.txt`.
 */

#endif // DOXYGEN_ONLY
