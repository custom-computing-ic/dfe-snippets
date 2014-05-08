# About

This implements a FIFO buffer of fixed length (as shift register).
Push and Pop operations are performed same time. Data movements done
with 1 cycle latency.

# Parameters

Length of this buffer is a compile time parameter.

# Example:


 * Parameters: buffer size = 8

 * Input:  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 
 * Output: 0  0  0  0  0  0  0  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 
