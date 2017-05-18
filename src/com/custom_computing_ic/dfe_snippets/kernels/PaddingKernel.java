package com.custom_computing_ic.dfe_snippets.kernels;

import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.CounterChain;

/**
 * Pads the inputs with 0 after nInputs have been processed.
 *
 * Scalars:
 * NUM_INP - number of inputs that will be received from a previous kernel.
 * TOTAL_CYCLES - number of cycles that this kernel should give an output.
 *
 * @since 18/05/2017
 */
public class PaddingKernel extends Kernel {
  public static final String INP_NAME            = "PADDING_INP";
  public static final String OUT_NAME            = "PADDING_OUT";
  public static final String SCALAR_NUM_INP      = "NUM_INP";
  public static final String SCALAR_TOTAL_CYCLES = "TOTAL_CYCLES";

  /**
   * Constructor of the Padding Kernel.
   *
   * @param parameters parameters for this kernel passed from a manager
   * @param bitWidth number of bits to be processed in each cycle
   * @param dbg flag to decide whether to output debug information
   */
  protected PaddingKernel(KernelParameters parameters, boolean dbg) {
    super(parameters);

    DFEVar nInputs = io.scalarInput(SCALAR_NUM_INP, dfeUInt(32));
    DFEVar totalCycles = io.scalarInput(SCALAR_TOTAL_CYCLES, dfeUInt(32));

    CounterChain chain = control.count.makeCounterChain();
    DFEVar cycles = chain.addCounter(totalCycles, 1);
    DFEVar paddingCycles = cycles >= nInputs;

    DFEVar input = io.input(INP_NAME, dfeRawBits(bitWidth), ~paddingCycles);
    DFEVar out = paddingCycles ? 0 : input;
    io.output(OUT_NAME, out, dfeRawBits(bitWidth));

    if (dbg) {
      debug.simPrintf("PADDING: %d (%d) nInputs %d\n", cycles, totalCycles, nInputs);
    }
  }
}
