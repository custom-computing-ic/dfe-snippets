
 This is a placeholder to remind about a snippet that should illustrate
 linking DFE acceleration code as a third party library to a (generally
 larger) user code that is subject to acceleration.
 
 Features to implement:
  - compile DFE code to .so library rather than to a binary executable
    (add DFE code makefile)
  - present example user code that links DFE code as external .so lib
    (add user code makefile or CMake) and finally produces an executable
  - add bash script that wraps running target executable and starts/stops
    MaxOS deamons before and after running user code. Ideally to pass
    bash script cmd parameters to an executable.