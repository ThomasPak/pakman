# Contributing to Pakman

Welcome and thanks for taking the time to contribute to Pakman! :smile:

Pakman is an open source project and we invite all kinds of contributions.
This includes (but is not limited to) submitting bug reports/fixes, adding
examples, and implementing additional ABC algorithms.  If you contribute any
code, be sure to follow our [coding conventions](#coding-conventions).

Please note that this project and everyone participating in it is governed by
our [Code of Conduct](CODE_OF_CONDUCT.md).  When you contribute to Pakman, you
are expected to uphold this code.  Please report any unacceptable behaviour to
[pakman.pABC@gmail.com](mailto:pakman.pABC@gmail.com).

## How to contribute

### Submitting bug reports/fixes

If you find a bug in Pakman, check first whether the bug has already been
reported in [Issues](https://github.com/ThomasPak/pakman/issues).  If you
cannot find an open issue addressing the problem, [open a new
one](https://github.com/ThomasPak/pakman/issues/new).  Please ensure that you
use a descriptive title and include example code where necessary (e.g. for user
executables) that is [**minimal** and
**reproducible**](https://stackoverflow.com/help/minimal-reproducible-example).

If you can fix the problem yourself, [create a pull
request](https://github.com/ThomasPak/pakman/compare).

### Adding examples

Pakman is an extremely generic tool for approximate Bayesian computation and
can be used in a wide variety of contexts.  If you have used Pakman in an
application area that is currently not included in the examples and you wish to
share this with the world, please do so!

If your application can be implemented as a short and self-contained example,
please add it to the `examples` folder and [create a pull
request](https://github.com/ThomasPak/pakman/compare).  In addition, you must
add a tutorial page to the Pakman wiki.  **Pull requests for examples will only
be accepted once its accompanying tutorial page has been written.**

Alternatively, if you are using a codebase that is too large to pull into
Pakman, you can add a tutorial to the Pakman wiki directly without contributing
code.  In this case, you must ensure that the tutorial includes instructions on
how to obtain the code for your example.

If in doubt as to whether to pull in your code to the Pakman repository or not,
please [open an issue](https://github.com/ThomasPak/pakman/issues/new).

### Implementing new ABC algorithms

Additional ABC algorithms can easily be added to Pakman by subclassing the
[AbstractController](https://codedocs.xyz/ThomasPak/pakman/class_abstract_controller.html)
class.  For detailed instructions, see the documentation page [Implementing a
Controller subclass](https://codedocs.xyz/ThomasPak/pakman/controller.html).

Please clearly document any code you contribute.  Every class must have its
public and protected members documented with
[Doxygen](http://www.doxygen.nl/index.html) comments.

In addition, you must add tests in the `tests` directory for your Controller
with
[CTest](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest).
The tests should cover every combination of your Controller with every Master,
as well as every type of simulator (standard, MPI simulator in C and MPI
simulator in C++).  If you are not sure what is meant by an MPI simulator,
please see [this
wiki](https://github.com/ThomasPak/pakman/wiki/Example:-epithelial-cell-growth)
and [this documentation
page](https://codedocs.xyz/ThomasPak/pakman/mpi-simulator.html).

When you are ready to contribute your new Controller, please [create a pull
request](https://github.com/ThomasPak/pakman/compare).

### Rest

If you want to make any other type of contribution, please [open an
issue](https://github.com/ThomasPak/pakman/issues/new) first describing what
you plan to add or change, as well as why you want to make the changes.  If we
approve the proposed changes, you can contribute your code by [creating a pull
request](https://github.com/ThomasPak/pakman/compare).

You are also welcome to open issues to request features or make other
suggestions.

## Coding conventions

As a general rule of thumb, look at the existing code and try to code in the
same style.  Our coding conventions are:

* We indent using four spaces (soft tabs).
* We use the [Allman indentation
  style](https://en.wikipedia.org/wiki/Indentation_style#Allman_style).
* The maximum line length is 79.
* C++-style comments for normal comments.
* C-style comments for Doxygen documentation.
* We always put spaces around binary operators, e.g. `x += 1`, not `x+=1`.
* We use `PascalCase` for class names, `camelCase` for member functions and
  `snake_case` for everything else.
