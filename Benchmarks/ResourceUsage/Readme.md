# About

This design is used to benchmark the resource usage of basic
operations on various Maxeler boards.

You can find the annotated resource usage for each kernel (using
__MaxCompiler 2013.2.2__) in `src/ResourceUsageKernel.<DFEModel>`.

# Input

  * a, b - a stream of 32 bit signed (two's complement) integer values

# Output

  * intResult, spResult, dpResult - 32 bit int, single and double
    precision results of performing (the same operations) on the input
    streams a and b;
