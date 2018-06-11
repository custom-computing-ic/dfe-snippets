package com.custom_computing_ic.dfe_snippets.kernels;

import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.CounterChain;

/**
 * Un-pads the inputs and will not output zeros after
 * nInputs have been processed.
 *
 * Scalars:
 * NUM_INP - number of input data that are valid for output
 * TOTAL_CYCLES - number of cycles that this kernel should
 * receive data from the previous kernel
 *
 * @since 18/05/2017
 */
public class UnpaddingKernel extends Kernel {
  public static final String INP_NAME = "UNPADDING_INP";
  public static final String OUT_NAME = "UNPADDING_OUT";
  public static final String SCALAR_NUM_INP = "NUM_INP";
  public static final String SCALAR_TOTAL_CYCLES = "TOTAL_CYCLES";

  public UnpaddingKernel(KernelParameters parameters, int bitWidth,
                         boolean dbg) {
    this(parameters, bitWidth, 1, dbg);
  }

  /**
   * Constructor of the Unpadding Kernel.
   *
   * @param parameters parameters for this kernel passed from a manager
   * @param bitWidth number of bits to be processed in each cycle
   * @param dbg flag to decide whether to output debug information
   */
  public UnpaddingKernel(KernelParameters parameters, int bitWidth,
                         int numSplit, boolean dbg) {
    super(parameters);

    DFEVar nInputs = io.scalarInput(SCALAR_NUM_INP, dfeUInt(32));
    DFEVar totalCycles = io.scalarInput(SCALAR_TOTAL_CYCLES, dfeUInt(32));
    CounterChain chain = control.count.makeCounterChain();
    DFEVar cycles = chain.addCounter(totalCycles, 1);
    DFEVar unpaddingCycles = cycles >= nInputs;

    DFEVar input = io.input(INP_NAME, dfeRawBits(bitWidth));

    if (numSplit == 1) {
      io.output(OUT_NAME, input, dfeRawBits(bitWidth), ~unpaddingCycles);
    } else {
      for (int i = 0; i < numSplit; i++) {
        if (bitWidth % numSplit != 0)
          throw new IllegalArgumentException(
              "bitWidth % numSplit should equal to 0");

        int splitBitWidth = bitWidth / numSplit;

        io.output(OUT_NAME + "_" + i,
                  input.slice(i * splitBitWidth, splitBitWidth),
                  dfeRawBits(splitBitWidth), ~unpaddingCycles);
      }
    }

    if (dbg) {
      debug.simPrintf("UNPADDING: %d (%d) nInputs %d\n", cycles, totalCycles,
                      nInputs);
    }
  }
}
