**********************************************************
* README V1.1
*
* Quantitative Phase Image Processing (QPI)
* Author: Junyi Xie
*         Dept. of Electrical and Electronic Engineering         
*         University of Hong Kong
*         Xinyu Niu
*         Dept. of Computing         
*         Imperial College London
* 
**********************************************************
  This is the maxeler FPGA accelerated version of Quantitative Phase Image Processing.
  Image size:
    256 * 256
  
  Current notices:
    Fixed point number is (25, 15) format now.
    Precision loss in critical modules.
  
  Current Clock Frequency:
    150MHz

  Current throughputs on FPGA:
    32-bit float input:
      120000 images in ~67 seconds
    16-bit signed integer input:
      120000 images in ~60 seconds
  
  CPU version on E5 2640 6 cores, 6 threads, HT turned off:
    120000 images in ~420 seconds
