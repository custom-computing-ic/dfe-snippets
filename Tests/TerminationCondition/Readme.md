# About

Simple passthrough design to test termination condition for a blocking
SLiC call in the presence of memory streams.

According to the manual the blocking call should wait until all memory
streams have set the interrupt flag. Leaving a large number of bursts
unread can stall the kernel. This is probably because the data queue
from memory fills up and the last memory read command (which should set
the interrupt flag) is left unprocessed in the command queue.

This behaviour can be reproduced both in simulation and hardware with
this snippet at between 64 - 128 bursts queued and unprocessed.
