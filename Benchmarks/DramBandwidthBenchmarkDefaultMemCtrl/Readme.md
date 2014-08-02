# About

This is a DRAM bandwidth benchmark with default memory controller and linear access pattern.

We benchmark simple reads of a number of single precision vector entries. Vector
is large enough (>1GB) to neglect any setup/initialisation costs. Calls to
compute kernel are repeated several times to avoid run time fluctuations. We benchmark
both FPGA and CPU memory bandwidth. On CPU our vector size yields out-of-cache memory
throughput benchmark.

# Parameters

    - Number of vector entries

    - Bitwidth of vector entries

    - Kernel and memory frequency

# Example:

For the kernel and memory frequencies 180Mhz and 733Mhz accordingly (Maia), reading
48 and 96 floats (32 bit) per cycle gives the following figures:

48 float32 values = 32080.29 MiB/s
96 float32 values = 52270.56 MiB/s

Test data set size is 1464.843750 MiB, vector with 384*10^6 float32 entries. Tests are
repeated 50 times, time measured with microsecond(*) resolution with gettimeofday().

(*) not always accurate

