#ifndef HELP_H
#define HELP_H

#include <string>

#include "core/common.h"

/** @file help.h
 *
 * This file defines helper functions to display help messages on the
 * command-line.
 */

/** @return usage format. */
std::string usage();

/** @return explanation of general options. */
std::string general_options();

/** Print an overview of available help messages.
 *
 * @param status  exit status.
 */
void overview(int status);

/** Print help message when '-h' or '--help' flag is given.
 *
 * @param master  master type.
 * @param controller  controller type.
 * @param status  exit status.
 */
void help(master_t master, controller_t controller, int status);

#endif // HELP_H
