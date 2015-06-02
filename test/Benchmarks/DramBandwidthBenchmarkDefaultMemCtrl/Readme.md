# About

This is a DRAM bandwidth benchmark with default memory controller and linear access pattern.

We benchmark simple reads of a number of 64 bit integers per cycle split into 2
DRAM streams. Vector is large enough (>1GB) to neglect any setup/initialisation
costs. Calls to compute kernel are repeated several times to avoid run time
fluctuations.

# Parameters

    - Number of vector entries

    - Bitwidth of vector entries

    - Kernel and memory frequency

# Experience:

For the kernel and memory frequencies 180Mhz and 733Mhz accordingly (Maia), reading
48 and 96 floats (32 bit) per cycle in a single DRAM stream gives the following figures:

48 float32 values = 32080.29 MiB/s
96 float32 values = 52270.56 MiB/s

Test data set size is 1464.843750 MiB, vector with 384*10^6 float32 entries. Tests are
repeated 50 times, time measured with microsecond(*) resolution with gettimeofday().



For the kernel frequency 190Mhz and DRAM freq 733Mhz reading two 1536 bits vectors 
per cycle gives:

2 streams (24 entries 64 bit each; in total 384 bytes): 58.11 GB/s

on 24GB dataset.

(*) not always accurate

