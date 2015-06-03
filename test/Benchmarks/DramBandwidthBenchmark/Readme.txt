<< Readme >>
This is a set suite for evaluation of memory access speed using custom memory commands and linear access patterns.
The code fetches parallel data streams from LMEM and then routes to LMEM and/or host which can be set by user.
The code parameterizes the major configuration users will change when using custom memory command
(e.g. burst per command, size of memory to access, width of memory interface, number of parallel LMEM streams)
The code may also serves as a reference for fresh starters in Maxeler for its simplicity, which their work may
develop upon if their design requires LMEM access.


<< Compilation >>
Build: make sim (for simulation) / make dfe (for hardware)
Run:   make run-sim (for simulation) / make run-dfe (for hardware)
Stop:  make stop-sim (for simulation) / make stop-dfe (for hardware)
Target folder: "mem_test_Sim" or "mem_test_DFE" + <BUILD TAG> (a Makefile parameter, see parameter section)
Host program / Makefile will look at the host name and automatically determine whether to run MAX3 or MAX4 for hardware.

NOTE: If the simulation process / hardware command is stopped before completion (ctrl-c), process may build up in the
host / hardware system will be locked up and other users cannot use it.
SO REMEMBER TO STOP PROCESS AFTER RUNNING SIMULATION / HARDWARE.


<< User Parameters >>
- Host code (mem_test.c)
    MEM_TO_MEM: Write content back to LMEM after reading them from LMEM
    MEM_TO_HOST: Send LMEM content to host after reading them from LMEM
    RAND_ACCESS: Random LMEM memory access addresses
    MEM_STREAMS: Number of parallel command / memory streams to use
    BURSTS_PER_CMD: Number of requested bursts per command
    DRAM_BURSTS: Total size of data to read from LMEM in bursts
	
- DFE Manager (DFEManager.java): 
    MAX_STREAMS: Number of parallel command / memory streams available, decrease to meet timing
    MEM_STM_WIDTH: Width of the memory interface in DFE, decrease to meet timing
    LMEM_LINEAR_ACCESS: Use linear access pattern instead of custom memory commands
    QUARTER_RATE: set to true to enable memory quarter rate mode, only available in MAX4 MAIA, may lower clock
    OPTIMIZATION: Enable a few configuration options

- Simulation Manager (SimManager.java)
    MAX_STREAMS, MEM_STM_WIDTH, LMEM_LINEAR_ACCESS (same as DFE Manager)

- Makefile
    BUILD_TAG: a tag to name target build folder, for user to build different hardware configurations
    BUILD_CARD: MAX3424A (MAX3) or MAX4848A (MAX4)


<< LMEM Performance >>
MAX4 (Stream clock: 150MHz, DRAM clock: 800MHz, quarter rate mode)
Memory speed:
  Read only, bpc = 2/64/128:    29GB/s(#)
  Read only, bpc = 1:           19GB/s
  R/W, bpc = 64/128:            29GB/s x 2
  R/W, bpc = 2:                 12GB/s x 2
  R/W, bpc = 1:                 7.4GB/s x 2


MAX4 (Stream clock: 150MHz, DRAM clock: 533MHz)
Memory speed:
  Read only, bpc = 2/64/128:      28GB/s(#)
  Read only, bpc = 1:             21GB/s
  R/W, bpc = 128:                 23GB/s x 2
  R/W, bpc = 64:                  22GB/s x 2
  R/W, bpc = 2:                  9.6GB/s x 2
  R/W, bpc = 1:                  6.8GB/s x 2


MAX3 (Stream clock: 120MHz, DRAM clock: 400MHz)
Memory speed:
  Read only, bpc = 2/64/128:      23GB/s(#)
  Read only, bpc = 1:             17GB/s
  R/W, bpc = 128:                 18GB/s x 2
  R/W, bpc = 64:                  17GB/s x 2
  R/W, bpc = 2:                  8.8GB/s x 2
  R/W, bpc = 1:                  5.4GB/s x 2


Max tested total bandwidth (multiple streams used^):
  MAX3 (400MHz DRAM)              36GB/s
  MAX4 (533MHz DRAM)              46GB/s
  MAX4 (QR mode 800MHz DRAM)      65GB/s


Max bandwidth for single read stream^
  MAX3 (400MHz DRAM)              23GB/s
  MAX4 (533MHz DRAM)              28GB/s
  MAX4 (QR mode 800MHz DRAM)      29GB/s


FPGA-to-Host Speed (PCI/infiniband)(*)
  MAX3                       1.3-1.4GB/s
  MAX4                           0.6GB/s


bpc: bursts per command
R/W: Write back to memory after reading
FPGA DRAM interface width = 1536 bits, which may not meet timing for larger design
Results accounted for overhead time in filling pipeline / FPGA set up

# capped by stream clock rate, 64/128 bpc should result in higher speed using higher FPGA streaming clock
  (e.g. For MAX4, 150MHz * 1536bits (max bus width) = 28.8GHz)
* looks too slow, need further testing
^ achieved using large burst size, will be lower using smaller burst size (e.g. under 64)


<< Summary & LMEM Usage Advice >>
- You can get most bandwidth using two streams using large burst size and input width
- Using quarter rate mode increases the total bandwidth considerably due to higher clock rate but not much performance
  benefit for a single stream
- Using sequential or random access pattern for access address in custom memory cmd (128 bpc) does not influence speed
- Recommended max bursts per command: 60-160 (64/128 are good choices), burst size > 200 generally results in slower speed
- Speed limit = interface width * clock rate, so when interface width is narrow, speed is capped by clock rate
  e.g. for a width of 384bits and streaming clock at 120MHz, each stream gets 5.7GB/s

  
<< Maxeler System Bug >> ***IMPORTANT***
- Bursts per command > 245 does not work in many cases, avoid using them
- For MAX4, ensure there is at least 1 cycle gap before commencing another command (a bug in MAX4)