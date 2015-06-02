# About

This snippet tests if one can declare a BRAM in one Kernel and
pass it to another Kernel via Manager. Important: these kernels
are derived from Kernel base class, not KernelLib.

As Kernels, they're instantiated and connected via Manager, so
in hardware they ought to work _asynchnously_. But how to manage
asynchonous access to shared BRAM without any semaphores?

So the answer is NO. Compiling this snippet gives you an error.
