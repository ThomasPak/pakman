#ifndef DOXYGEN_H
#define DOXYGEN_H

// This header contains no code.  It only exists to contain stand-alone Doxygen
// documentation pages.

/** @mainpage
 *
 * # Introduction           {#introduction}
 *
 * Welcome to the code documentation of Pakman!  If you stumbled here without
 * any context whatsoever, you should have a look at the Pakman repository
 * [here](https://github.com/ThomasPak/pakman) first.  There, you can find
 * high-level instructions for installing and using Pakman.
 *
 * These pages contain documentation generated from the source files of the
 * Pakman project, as well as some advanced instructions that are not found in
 * the Wiki (e.g. how to extend Pakman).  For an overview of the classes and
 * files defined in the Pakman project, see the sidebar on the left.
 *
 * # Organisation           {#organisation}
 *
 * The Pakman source files are organised into seven different subdirectories:
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
 * a separate communication protocol for MPI simulators.  See [this Wiki
 * page](https://github.com/ThomasPak/pakman/wiki/Example:-epithelial-cell-growth)
 * for an example of an MPI simulator and see
 * @ref mpi-simulator "this documentation page" for instructions on how to
 * implement an MPI simulator.
 *
 * If you wish to implement an ABC algorithm that is currently not part of
 * Pakman, please see @ref controller "this link" on how to write a new
 * Controller subclass.
 */

/** @page mpi-simulator Implementing an MPI simulator
 *
 * Here are some instructions on implementing an MPI simulator.
 */

/** @page controller Writing a new Controller subclass
 *
 * Here are some instructions on writing a new Controller subclass.
 */

#endif // DOXYGEN_H
