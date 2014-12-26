# About

Simple passthrough design to test termination condition for a blocking
SLiC call in the presence of memory streams. According to the manual
the blocking call should wait until all memory streams have set the
interrupt flag.

We run the kernel for N cycles and expect N output values. We queue
2 * N data from on board DRAM. According to the manual this should
stall, but it seems to work in simulation.
