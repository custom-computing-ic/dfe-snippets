# About

This design tests if automatic stall of a kernel A works when kernel A is connected to kernel B and kernel B is doing twice more work.

# Example:

Imagine matrix-vector computation M*(v+w) where kernel A adds two vectors v and w while kernel B multiplies matrix M to the output of kernel A.
For every pair of vector elements processed by kernel A in a kernel cycle, kernel B needs to process whole matrix row. Thus,
kernel B needs to read an input vector conditionally. Does kernel A automatically stalls as required by conditional input of kernel B?
Here we test it.