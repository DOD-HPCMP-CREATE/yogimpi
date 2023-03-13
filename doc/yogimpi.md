---
author:
- Stephen Adamec
title: YogiMPI Installation, Usage, and Project Integration
---

# Background

YogiMPI allows developers to build a single MPI-based binary that will
run on most systems with the same operating system and basic
architecture (in all uses so far, x86_64 Linux). The following
instructions detail how to build YogiMPI, how to work with any generic
MPI application, and specific instructions to compile the HELIOS
application.

There are some other important things to consider in order to maximize
the number of systems where you can use a YogiMPI-built binary. Please
see the last section, [Choosing a Build System](#choosebuild), for more
information.

# Setup and Using YogiMPI

## Choose A Compiler and MPI

YogiMPI requires an Intel or GNU compiler in order to build properly. It
also requires an MPI-2 distribution against which to compile. Tested
distributions include MPICH, OpenMPI, SGI MPT, CRAY MPICH, and Intel
MPI.

The compiler choice is important. If a compiler is used to build a
binary with YogiMPI, that same compiler with an equal or greater version
must be present on any system where the binary will run.

Note that the application that is compiled with and linked against
YogiMPI is not dependent on any specific MPI distribution, allowing more
flexibility than traditional parallel applications.

## Build YogiMPI

1.  Get the YogiMPI source:

    `git clone git@github.com:DOD-HPCMP-CREATE/yogimpi.git`

2.  Check out a tag, if preferred:

    `git checkout v1.3.1`

3.  Load your chosen compiler, MPI distribution. YogiMPI also requires Python >= 3.7.

4.  Set the following environment variables. The examples use bash
    syntax, but \[t\]csh also works:

    1.  `YCC`: serial C compiler (e.g. `export YCC=icc` or `YCC=gcc`)

    2.  `YCXX`: serial C++ compiler (e.g. export `YCXX=icpc` or
        `YCXX=g++`)

    3.  `YF90`: serial Fortran compiler (e.g. `export YF90=ifort` or
        `YF90=gfortran`)

    4.  `YMPICC`: parallel C compiler (e.g. `export YMPICC=mpicc` or
        `YMPICC=cc`, the last being what you'd use on a CRAY)

    5.  `export YMPI_ENABLE_PYTHON=1`

5.  `./configure -i /path/to/install/yogimpi`

6.  `make`

7.  `make install`

## Compiling Any Generic Application with YogiMPI {#generic:compile}

YogiMPI comes with pre-installed compiler wrappers named `mpicc`,
`mpicxx`, `mpif77`, and `mpif90`. These may be used just like any other
compiler wrapper with your application. This will allow you to build the
application only once on your local *build system*, after which it can
be deployed to *target systems*.

In order to set up your build system environment so Yogi is ready to
compile:

1.  Load the compiler used to build YogiMPI.

2.  Load the MPI distribution used to build YogiMPI.

4.  Load YogiMPI. This can be achieved by

    `source /path/to/install/yogimpi/etc/yogimpi.bashrc`

    or by loading the module

    `module load /path/to/install/yogimpi/etc/modulefile`.

    It is **very important** that at the end of setting up your shell,
    `which mpicc` resolves to the YogiMPI wrapper version, not the MPI
    distribution version.

Now `mpicc`, `mpif90`, and `mpicxx` can be used normally to build an
application. If an application expects additional information about the
MPI directory loation, such as `MPI_ROOT`, this can be set to
`/path/to/yogimpi-install`. You may notice that if your application
contains many Fortran files with lots of MPI subroutine calls, a compile
with YogiMPI may take longer. This is because YogiMPI must preprocess
Fortran source more heavily than C and C++.

## Running Yogi-Built Applications on Another System

Once you have compiled your binary on the build system, you will want
the ability to run it elsewhere. As a reminder, if the build system was
where the binary was originally compiled, the *target system* is another
system where the executable is expected to run. In order to prepare a
target system, you'll need to do the following:

1.  Compile YogiMPI on the target system. You can use any MPI
    distribution you want; but the compiler vendor must be the same, and
    the version must be **equal or greater than** the one you used on
    the build system. For example, if you used Intel 12 on your build
    system, you must use Intel 12 or greater on the target system.

2.  Set up your shell or queue script with the compiler, MPI, and
    ptoolsrte configuration of your choice. The build of YogiMPI on the
    target system will contain an `etc/yogimpi.bashrc` and
    `etc/modulefile`, one of which must be sourced or loaded,
    respectively.

3.  Run the application normally, as if it was built with the local MPI.

# Building a Project with YogiMPI

## Compile MPI-using dependencies with YogiMPI

# Extra: Choosing a Build System {#choosebuild}

Several factors are involved in choosing an ideal build system that
maximizes compatibility of a YogiMPI binary. A *build system* refers to
a specific workstation or server where an application will be compiled
with YogiMPI. A *target system* refers to any system where this
application will be deployed for execution.

## glibc Compatibility

One major issue in binary compatibility between Linux systems is the
version of the GNU C Library (glibc) installed by the operating system.
If a YogiMPI binary is created on a build system, the glibc version on
the target system **must be equal to or greater than** the build
system's glibc version. For example, SuSE Enterprise Linux 11 has a
lower glibc version than CentOS 6 systems. We can verify this by
checking the version on SLES 11:

    sadamec@ozymandias:~> cat /etc/SuSE-release
    SUSE Linux Enterprise Server 11 (x86_64)
    VERSION = 11
    PATCHLEVEL = 1
    sadamec@ozymandias:~> ls -al /lib64/libc.so*
    lrwxrwxrwx 1 root root 14 2014-02-27 14:10 /lib64/libc.so.6 -> libc-2.11.1.so

And on CentOS 6:

    sadamec@me-saala ~]$ cat /etc/redhat-release
    CentOS release 6.8 (Final)
    [sadamec@me-saala ~]$ ls -al /lib64/libc.so*
    lrwxrwxrwx. 1 root root 12 Jun  2 10:38 /lib64/libc.so.6 -> libc-2.12.so

Therefore if the build system were SLES11, the build would run on a
target system that is CentOS 6; but the reverse is not true. In general,
choosing the **lowest glibc version** will result in greater
compatibility.

## Compiler Optimizations

Another problem can occur when using chip-specific optimizations on a
YogiMPI binary. If the build system contains a newer chipset than a
target system, using optimizations for that chipset may result in a
binary that does not run on older hardware. For instance, using the
Intel compiler `-xHost` flag on a build system that is newer than a
deployment target might result in code that cannot execute. There are
several solutions to this problem:

-   Do not use chip-specific optimizations. The default hardware
    optimization of the Intel compiler will likely target SSE3
    instructions, which have been in hardware for the last decade. Non
    architecture-specific optimization flags such as `-O3` are still
    acceptable.

-   Use older hardware for a build system that is guaranteed to meet the
    minimum hardware requirements. Then a flag like `-xHost` will
    provide some optimizations, but it will remain compatible with all
    systems.

-   As a variation of the above, create a virtual machine that reports
    an older chipset than the actual hardware, which will again end with
    lower chip optimizations.

-   If the compiler supports it, use multiple optimization pathways.
    Intel compilers offer options to compile for multiple architectures,
    and the highest supported is chosen at runtime.
