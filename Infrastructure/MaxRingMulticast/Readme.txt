<< Readme for MaxRing Multi-cast Kernel >>

============================ A. Introduction =============================

 This project uses the MaxRing to boardcast fixed size data to other cards
 in a single Maxeler node.

 The user will first need to set the number of data each FPGAs will send to
 other cards through scalar input, each FPGA will then send a stream of
 data to FPGAs with larger index (right) in batches, after which the data
 will then be sent to the FPGAs with smaller indices (left). Separating the
 two directions prevents deadlock among FPGAs.

 To prevent the downstream computation starved of data input, the whole set
 of data is divided into a number of rounds, each consisting the rightward
 and leftward communication mentioned previously.

 The data going into this broadcasting kernel is currently a counter within
 the kernel itself, but it can be replaced by input from other kernels.
 Since the data will go around the MaxRing twice (rightward and leftward),
 the data is stored in BRAM buffer during the rightward phase and then read
 out during the leftward phase.

 Unlike the sample code provided by Maxeler, this code is scalable and you
 may specify any number of FPGAs to be used in the ring.



============================= B. Algorithm =============================

 Illustraive Example (4 cards):
 Data sent by card 0/1/2/3 are denotes by A/B/C/D respectively.
 '-' denotes idle cycle.
 Each FPGA sends out 1 data. A total of [numFPGAs*dataPerRound*2] cycles
 (ticks) are run.

 The actual data to be sent/received will be in the following format:
 <valid bit>, <source FPGA idx>, <actual data>
 with the valid bit being the most significant bit.


 >> Data to send to other FPGAs (right/left)
  Card idx:     0   1   2   3
  cycle 1(R):   A   A   A   -
  cycle 2(R):   -   B   B   -
  cycle 3(R):   -   -   C   -
  cycle 4(R):   -   -   -   -
  cycle 5(L):   -   D   D   D
  cycle 6(L):   -   C   C   -
  cycle 7(L):   -   B   -   -
  cycle 8(L):   -   -   -   -

  Cycle 1-4: Data send to right (larger FPGA index)
  Cycle 5-8: Data send to left  (smaller FPGA index)


 >> Data to send to downstream computation of own FPGA
  Card idx:     0   1   2   3
  cycle 1(R):   A   A   A   A
  cycle 2(R):   -   B   B   B
  cycle 3(R):   -   -   C   C
  cycle 4(R):   -   -   -   -
  cycle 5(L):   D   D   D   D
  cycle 6(L):   C   C   -   -
  cycle 7(L):   B   -   -   -
  cycle 8(L):   -   -   -   -

  Cycle 1-4: Data send to right (larger FPGA index)
  Cycle 5-8: Data send to left  (smaller FPGA index)


 >> Number of cycles counted by counter*
  Card idx                            0   1   2   3 
  1. data to receive/pass-on* to R    0   1   2   3 
  2. send own data to pass on to R    1   1   1   0
  3. idle#                            3   2   1   1
  4. data to receive/pass-on* to L    3   2   1   0
  5. send own data to pass on to L    0   1   1   1
  6. idle#                            1   1   2   3

 * Numbers = data sent in multiple of dataPerRound
 # 1 cardCnt cycle extra to account for the pipeline depth
 When all data is sent, valid bit to be sent is set to 0

 Right Phase = 1, 2, 3, Left Phase = 4, 5, 6


 >> Switch for each phase (refer to the previous chart)
  1. inL open, outR open
  2. outR open
  3. all close
  4. inR open, outL open
  5. outL open
  6. all close

  (inL/outL disabled for leftmost card, oposite for rightmost card)



======================= C. User and system Variables =====================

  User variables from CPUs are set through scalar input.
  System variables are parameters reside in kernels and cannot be changed
  during runtime.
  
  >> User Variables:
	dataPerRound:
	  Larger data count will increase efficiency, smaller count
	  causes higher overhead but downstream will not be "starved", thus
	  benefits computation that requires off-chip I/O (e.g. LMem, CPU data)
	  Since the data must be sent in multiple of 32 bytes and the default
	  data size is 32 bits, dataPerRound should be a multiple of 8. The
	  kernel will stall if it is not a multiple of 8.

	numTotalData:
	  TotalData to be sent from each FPGA. It can be different across the
	  FPGAs, but in this case the numTotalData used to calculate the total
	  number of cycles to run should be replaced as the maximum of all the
	  numTotalData when calling <max_set_ticks>:

	  kernel_ticks = numFPGAs*2 * floor((max(numTotalData_i)+dataPerRound-1)
	    / dataPerRound) * dataPerRound - dataPerRound;
	  
	numFPGAs:
	  Number of FPGAs to be used, 2 <= n <= maxCards 


  >> System Variables:
	counterWidth (default: 32)
	  The counterWidth for many general counters.

 	dataWidth (default: 28)
	  The width of the actual data to be sent. Since the data has to be
	  padded to 32 bytes in total, dataWidth should be [n*32-card_bits-1]
	  bits, where card_bits is 3 for 8 cards, and the 1 is for valid bit.

	maxCards (default: 8)
	  The maximum total number of cards in this node. Determines card_bits.

	maxDataPerRound (default: 5120)
	  The maximum data to be sent in 1 round. dataPerRound must be smaller
	  than maxDataPerRound.



================== D. Integration with your own design =================

  This MaxRing kernel will accept [numTotalData] inputs and outputs
  [numFPGAs*numTotalData] outputs or [sum(numTotalData_i)] if the FPGAs
  have different number of data.

  >> Input:
    Replace the counter input (line 153-156 in Kernel) with input from
	another	kernel (line 147), then connect them in Manager.

  >> Output:
    Replace the output connection to CPU in Manager to your desired
	kernel (line 39 in Manager).



=========================== E. Performance ===========================

  Using large values of dataPerRound will give faster result in general,
  up to 2048 dataPerRound.

  Performance of each FPGA sending out 76800000 32 bits data, 6 FPGAs:
  Using 5120 data per round -> 12.0s, 153MB/s
  Using 2048 data per round -> 11.2s, 164MB/s (optimal)
  Using 1024 data per round -> 11.8s, 156MB/s
  Using 512 data per round  -> 13.2s, 139MB/s
  Using 128 data per round  -> 21.5s, 85MB/s
  Using 8 data per round    -> 31.1s, 59MB/s

  Bandwidth calculated is for single inter-FPGA link and includes sending
  and receiving data (e.g. FPGA0 send out 1 data and receive 5 data in
  return, while FPGA1 send out 2 data and receive 4 data)
  Bandwdith formula: totalData * data size* numFPGAs / time



========================= G. Important Notes  ==========================

- Behavioural simulation cannot simulate MaxRing. In the Manager code a
  number of kernels are instantiated within a single FPGA and CPU call
  a single FPGA instead. Thus line 45 should be set to TRUE during sim
  and to FALSE for actual DFE compilation.

- Total data transferred from one FPGA to another must be in multiple of
  32 bytes, probably due to caching of inter-FPGA interface. Thus 
  [(dataWidth+cardBits+1)*dataPerRound] should be a multiple of 256
  (i.e. 32 bytes)


