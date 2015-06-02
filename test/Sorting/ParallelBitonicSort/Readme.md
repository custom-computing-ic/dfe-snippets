# About

This is a bitonic sorter design for multiple DFEs using OpenMP.

Each DFE is loaded with one (variable input count) bitonic sorter
kernel and allocated a chunk of the input.

For a detailed description of the bitonic sorting network see this
[article](http://en.wikipedia.org/wiki/Bitonic_sorter) and
["Sorting networks and their applications", K.E. Batcher](http://dl.acm.org/citation.cfm?id=1468121).

# Parameters
  * networkWidth - number of inputs to the bitonic network; this
    must be a __power of 2__.

# Input
  The input is a stream of integer numbers to sort.
  __Note__ The number of elements in the input stream must be a power of 2.

# Output
  Sorted input stream.

# Example:

 * Parameters:
    * networkWidth = 4

 * Input:
    *  in  = {4, 7, 6, 5, 3, 2, 1, 8}

 * Output:
    * out = {1, 2, 3, 4, 5, 6, 7, 8}
