# About
This design is a scalable (up to 128 inputs) bitonic sorting network.

For a detailed description see this
[article](http://en.wikipedia.org/wiki/Bitonic_sorter).

# Parameters
  networkWidth - number of inputs to the bitonic network

# Input
  The input is a number of vectors of size networkWidth.

# Output
  Each vector is sorted in increasing order.

# Example:
  For networkWidth = 4, nVectors = 2:

  Input could be:
    in  = {{4, 7, 6, 5}, {3, 2, 1, 4}}

  Output should be:
    out = {{4, 5, 6, 7}, {1, 2, 3, 4}}
