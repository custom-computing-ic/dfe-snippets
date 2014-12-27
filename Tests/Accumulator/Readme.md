# About

This is a simple example of the Maxeler Accumulators. It tests
that it supports fixed point accumulation and does not support
floating point.

# Input

 * int_in - a stream of ints to sum
 * fixed_in - a stream of fixed point numbers to sum

# Output

  * int\_out - for each cycle the accumulated sum of the processed
    value from the int_in stream
  * fixed\_out - for each cycle the accumulated sum of the processed
    value from the fixed_in stream

# Example:

 * Input:
    * int_in = {1, 2, 3}
    * fixed_in = {1.5, 2.5, 3.5}

 * Output:
    * int_out = {1, 3, 6}
    * fixed_out = {1.5, 4, 7, 5}
