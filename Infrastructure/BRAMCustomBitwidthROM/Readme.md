# About

Here we check if data bit width conversion happens on the fly correctly
for the 64bit wide integer data mapped from CPU to 12bit wide integers
in BRAM/ROM. Also handle reading 2 entries from mapped BRAM same cycle.

In this example we store some values to the kernel's local BRAM, read
read and add values per cycle from that BRAM/ROM, sending the results
back to CPU for comparison.

# Input

  rom - a stream of 64bit wide integers being mapped to FPGA's BRAM
  via Maxeler API. Internally these integers are stored 12bit wide.

# Output

  b - a stream of 32 bit integers; b[i] := rom[2*i] + rom[2*i+1]
