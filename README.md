YogiMPI Overview
================

* Prerequisites *
  - Intel or GNU (4.8+) C, C++, and Fortran compiler
  - MPI distribution with compiler wrappers (e.g. mpicc, mpicxx, mpif90)
  - MPI distribution must support at least the MPI v2.1 API.

* Installation *
  - Load your compilers and MPI wrappers into the path and runtime library path.
  - Choose an MPI API version to support. Current options are 2.1, 2.2, and 3.
    Set the YVERSION variable to any of these values. The default is 2.1.
  - Set the environment variable YCC, YCXX, and YF90 to point to your serial
    compilers.
  - As an alternative to the above compiler variables, you may also specify
    YFAMILY=gnu or YFAMILY=intel, which will set the variables to the default
    values of gcc, g++, and gfortran for GNU and icc, icpc, and ifort for
    Intel. This also sets some compiler flags.
  - If you are using the GNU Fortran compiler without the YFAMILY variable, you
    will need to add -ffree-line-length-none to FFLAGS in Make.flags for proper
    compilation.
  - Set the environment variable YMPICXX to point to your MPI's C++ compiler
    wrapper.
  - YogiMPI has optional debugging output enabled by setting the YDEBUG
    environment variable to 1. This will create a yogimpi.log.<global_rank>
    file for each MPI rank, and it will list functions entered and exited
    by the rank. Use only when required. This can be useful when you create
    a "release" and "debug" version of Yogi as they can be swapped out for each
    other as needed.
  - Once set, run ./configure -i /path/to/install
  - To build, type "make," and then "make install" to copy files to the
    installation directory.
  - Note that some MPI deprecation warnings may be given.  These may be ignored.

* Testing *
  - A small set of C and Fortran tests ship with YogiMPI to ensure that basic
    functionality works as expected.  This is *not* a comprehensive test suite
    but a small subset of commonly used MPI routines.
  - After "make" and "make install" (the installation *must* run), change into
    the test subdirectory.
  - "make runtest" will build and execute all tests.  If successful, a message
    will be printed.

* Modules and Source Files *
  - As part of the installation, module files and a bash script are provided
    in /path/to/install/etc.  These can be used to add YogiMPI to the PATH and
    [DY]LD_LIBRARY_PATH.

* Version Numbering *
  - An release candidate increase (1.1.1rc1 -> 1.1.1rc2) is a minor bug fix or
    similar change in preparation for a new release.
  - An example of a patch release (1.1.Z) would be adding support for an MPI
    function without changing existing behavior.
  - An ABI change requires incrementing the minor release number (1.Y.0).
  - A major release increment (X.0.0) would only be for a major interface
    change.
