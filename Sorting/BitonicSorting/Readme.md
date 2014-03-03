# About
The kernel is a scalable (up to 128 inputs) bitonic sorting network.
This is used to sort nVectors vectors of size networkWidth each.

For a detailed description of the bitonic sorting network see this
[article](http://en.wikipedia.org/wiki/Bitonic_sorter) and
["Sorting networks and their applications", K.E. Batcher](http://dl.acm.org/citation.cfm?id=1468121).

# Parameters
  networkWidth - number of inputs to the bitonic network
  nVectors - number of vectors to sort

# Input
  The input is nVectors floating point vectors of size networkWidth.

# Output
  Each of the input vectors sorted in increasing order.

# Example:

 * Parameters:
    * networkWidth = 4, nVectors = 2:

 * Input:
    *  in  = {{4.0, 7.0, 6.0, 5.0}, {3.0, 2.0, 1.0, 4.0}}

 * Output:
    * out = {{4.0, 5.0, 6.0, 7.0}, {1.0, 2.0, 3.0, 4.0}}
