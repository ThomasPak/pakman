"""Copyright (c) 2005-2017, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

import os


def find_and_replace(filename, old_string, new_string):
    # Read the file
    f = open(filename, 'r')
    file_contents = f.read()
    f.close()

    # Write to the file
    f = open(filename, 'w')
    f.write(file_contents.replace(old_string, new_string))
    f.close()

    return


def ask_for_response(question):
    # Display the question
    print(question)

    # Define permitted yes/no answers
    yes = {'yes', 'y', 'ye', ''}
    no = {'no', 'n'}

    # Take the lower case raw input
    choice = raw_input().lower()

    # Decide on the choice
    if choice in yes:
        return True
    elif choice in no:
        return False
    else:
        ask_for_response("Please respond with yes or no:")


def main():
    # The absolute path to the project directory
    path_to_project = os.path.dirname(os.path.realpath(__file__))

    # Identify the name of the project
    project_name = os.path.basename(path_to_project)

    # Paths to the CMakeLists.txt files
    base_cmakelists = os.path.join(path_to_project, 'CMakeLists.txt')
    apps_cmakelists = os.path.join(path_to_project, 'apps', 'CMakeLists.txt')
    test_cmakelists = os.path.join(path_to_project, 'test', 'CMakeLists.txt')

    # Perform the find-and-replace tasks to update the template project
    find_and_replace(base_cmakelists, 'chaste_do_project(template_project', 'chaste_do_project(' + project_name)
    find_and_replace(apps_cmakelists, 'chaste_do_apps_project(template_project', 'chaste_do_apps_project(' + project_name)
    find_and_replace(test_cmakelists, 'chaste_do_test_project(template_project', 'chaste_do_test_project(' + project_name)

    # Amend the components
    components_list = []

    if ask_for_response("Does this project depend on the cell_based component? [Y/n] "):
        components_list.append('cell_based')

    if ask_for_response("Does this project depend on the crypt component? [Y/n] "):
        components_list.append('crypt')

    if ask_for_response("Does this project depend on the heart component? [Y/n] "):
        components_list.append('heart')

    if ask_for_response("Does this project depend on the lung component? [Y/n] "):
        components_list.append('lung')

    # If the list is non-empty, replace the default components
    if components_list:
        components_string = ' '.join(components_list)
        default_components = 'continuum_mechanics global io linalg mesh ode pde'

        find_and_replace(base_cmakelists, default_components, components_string)


if __name__ == "__main__":
    main()
