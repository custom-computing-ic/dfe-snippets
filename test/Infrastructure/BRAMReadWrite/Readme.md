# About

Handle reading and writing back to BRAM in the same kernel by adding
some extra buffering to the address and input streams.

In this example we store some values to the kernel's local BRAM, read
them back and increment them, writing the results back to local
BRAM. After this is done the final results are streamed from the BRAM
to the CPU.

# Input

  a - a stream of floats

# Output

  b - a stream of floats; b[i] = a[i] + 1
