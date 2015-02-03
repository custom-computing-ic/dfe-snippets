
Summary
=======

 This snippet shows how to compile DFE code into a shared library and link it in
 to an external C/C++ project as a third party library.

 In this snippet, DFE project includes CPU part which provides an API for the
 external project, so that external CPU code has no idea what hardware is behind
 the API function it is calling. It's helps DFE code to act as a plug-in
 accelerator. External projects expects this API to be in include/ directory,
 and it's up to the DFE code developer to choose which functions to promote
 as API, thus include/*.h are not auto-generated.

 The DFE code can be compiled for both simulation and hardware, so in case of
 simulation the external code needs to be run via thin wrapper to start DFE
 simulation daemons. Hardware builds do not require wrapper, but it's handy for
 LD_LIBRARY_PATH fix on the fly so that external code finds the path to the
 DFE's .so library.

 Both DFE code and external projects could be either in C or C++.

 Directories
     build/
     include/
     src/
 belong to the DFE code while
     external_c/
     external_cpp/
 are the 2 examples of C and C++ applications that link against DFE code.

Compilation instruction
=======================

DFE code is using common makefiles for the whole dfe-snippets repository,
while sample external projects use simplifies own makefiles.

Compiling the .so files for simulation:

 cd LinkingThirdPartyLibrary/build/
 make simlib

Compiling the .so files for hardware:

 cd LinkingThirdPartyLibrary/build/
 make dfelib

Compiling the executable for the external project :

 cd ..
 cd external_c/build
 make sim

or

 make dfe

Running external project (in the same directory external_c/build/):

 ./simrunner ./TargetCPUCode_sim
 ./hwrunner ./TargetCPUCode_dfe

