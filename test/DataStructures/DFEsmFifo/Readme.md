# About

This is a thin wrapper around DFEsmFifo class provided
by Maxeler.

For some info about DFEsmFifo, follow the MDX post
https://groups.google.com/a/maxeler.com/forum/#!searchin/mdx/buffer/mdx/_sQ6PZwkk5Q/gLWcxD0qW8QJ

Few caveats:
- in some cases hardware compile fails if FIFO depth is too small (<16)
- it still occupies some BRAM even when declared with UseLUTRam
- readEnable signal enables for output at the NEXT cycle
- beware of data being out twice if FIFO is empty first time
  (though 'valid' output signal is 0 second time)

# Example:

Here's the debug output for FIFO of depth 4:

    cycle=0 | FIFO<-1 | FIFO->0, valid=0, readEnable=1, empty=1, full=0 | output=0 
    cycle=1 | FIFO<-2 | FIFO->1, valid=1, readEnable=1, empty=1, full=0 | output=1 
    cycle=2 | FIFO<-3 | FIFO->2, valid=1, readEnable=0, empty=1, full=0 | output=2 
    cycle=3 | FIFO<-4 | FIFO->2, valid=0, readEnable=0, empty=0, full=0 | output=0 
    cycle=4 | FIFO<-5 | FIFO->2, valid=0, readEnable=0, empty=0, full=0 | output=0 
    cycle=5 | FIFO<-6 | FIFO->2, valid=0, readEnable=0, empty=0, full=0 | output=0 
    cycle=6 | FIFO<-7 | FIFO->2, valid=0, readEnable=0, empty=0, full=1 | output=0 
    cycle=7 | FIFO<-8 | FIFO->2, valid=0, readEnable=1, empty=0, full=1 | output=0 
    cycle=8 | FIFO<-9 | FIFO->3, valid=1, readEnable=1, empty=0, full=0 | output=3 
    cycle=9 | FIFO<-10 | FIFO->4, valid=1, readEnable=1, empty=0, full=0 | output=4 
    cycle=10 | FIFO<-11 | FIFO->5, valid=1, readEnable=1, empty=0, full=0 | output=5 
    cycle=11 | FIFO<-12 | FIFO->6, valid=1, readEnable=1, empty=0, full=0 | output=6 
    cycle=12 | FIFO<-13 | FIFO->9, valid=1, readEnable=0, empty=0, full=0 | output=9 
    cycle=13 | FIFO<-14 | FIFO->9, valid=0, readEnable=1, empty=0, full=1 | output=0 
    cycle=14 | FIFO<-15 | FIFO->10, valid=1, readEnable=1, empty=0, full=0 | output=10 
    cycle=15 | FIFO<-16 | FIFO->11, valid=1, readEnable=1, empty=0, full=0 | output=11 
    cycle=16 | FIFO<-17 | FIFO->12, valid=1, readEnable=1, empty=0, full=0 | output=12 
    cycle=17 | FIFO<-18 | FIFO->13, valid=1, readEnable=1, empty=0, full=0 | output=13 
    cycle=18 | FIFO<-19 | FIFO->15, valid=1, readEnable=1, empty=0, full=0 | output=15 
    cycle=19 | FIFO<-20 | FIFO->16, valid=1, readEnable=1, empty=0, full=0 | output=16 
    cycle=20 | FIFO<-21 | FIFO->17, valid=1, readEnable=1, empty=0, full=0 | output=17 
    cycle=21 | FIFO<-22 | FIFO->18, valid=1, readEnable=1, empty=0, full=0 | output=18 
    cycle=22 | FIFO<-23 | FIFO->19, valid=1, readEnable=1, empty=0, full=0 | output=19 
    cycle=23 | FIFO<-24 | FIFO->20, valid=1, readEnable=1, empty=0, full=0 | output=20 

Note few values are lost due to FIFO overfill (watch 'full' output).