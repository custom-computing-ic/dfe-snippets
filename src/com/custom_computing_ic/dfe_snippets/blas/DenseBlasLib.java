package com.custom_computing_ic.dfe_snippets.blas;

import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVector;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.CounterChain;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.*;
import com.maxeler.maxcompiler.v2.utils.MathUtils;

import com.custom_computing_ic.dfe_snippets.utils.Reductions;

/** A library for Dense BLAS */
public class DenseBlasLib {

  /** A dense matrix vector multiplier, where the results are accumulated in a separate stream */
  public static class LMemDMVM extends KernelLib {

    private DFEVar res;
    private DFEVar readEnable, stripeCount, row;
    private final int maxBsize;

    LMemDMVM(KernelLib owner, DFEVar n, DFEVar nstripes, DFEVar iterations, int maxBsize) {
      super(owner);
      this.maxBsize = maxBsize;
      CounterChain chain = control.count.makeCounterChain();
      chain.addCounter(iterations, 1);
      this.stripeCount = chain.addCounter(nstripes, 1);
      this.row = chain.addCounter(n, 1);

      // only read on the first cycle of each stripe
      this.readEnable = row === 0;
    }

    /**
     * Perform a dense matrix vector multiplication.
     *
     * NB calling mult on the same DMVM multiple times will duplicate the
     * artithetic path, but not the control path. Consider carefully if this is
     * what you really want to do. */
    public void mult(DFEVector<DFEVar> a, DFEVector<DFEVar> v, DFEVar b) {
      // Previous b values are streamed
      DFEVector<DFEVar> mult = a * v;
      this.res = b + Reductions.reduce(mult.getElementsAsList());
    }

    public DFEVar getReadEnable() {
      return readEnable;
    }

    public DFEVar getRes() {
      return res;
    }
  }

  /** A dense matrix vector multiplier, where the results are accumulated in FMem */
  public static class FMemDMVM extends KernelLib {

    private DFEVar writeEnable, res;
    private DFEVar readEnable, stripeCount, row;
    private final int maxBsize;

    FMemDMVM(KernelLib owner, DFEVar n, DFEVar nstripes, DFEVar iterations, int maxBsize) {
      super(owner);
      this.maxBsize = maxBsize;
      CounterChain chain = control.count.makeCounterChain();
      chain.addCounter(iterations, 1);
      this.stripeCount = chain.addCounter(nstripes, 1);
      this.row = chain.addCounter(n, 1);

      // only read on the first cycle of each stripe
      this.readEnable = row === 0;
      this.writeEnable = stripeCount === (nstripes - 1);
    }

    /**
     * Perform a dense matrix vector multiplication.
     *
     * NB calling mult on the same DMVM multiple times will duplicate the
     * artithetic path, but not the control path. Consider carefully if this is
     * what you really want to do. */
    public void mult(DFEVector<DFEVar> a, DFEVector<DFEVar> v) {
      // Previous b values are accumulated in BRAMs
      DFEVector<DFEVar> mult = a * v;
      Memory<DFEVar> bmem = mem.alloc(dfeFloat(11, 53), maxBsize);
      DFEVar addr  = row.cast(dfeUInt(MathUtils.bitsToAddress(maxBsize)));
      DFEVar prevb = stripeCount === 0 ? 0 : bmem.read(addr);

      this.res = prevb + Reductions.reduce(mult.getElementsAsList());
      bmem.write(
          stream.offset(addr, -17),
          stream.offset(res, -17),
          stripeCount === stripeCount);
    }

    public DFEVar getReadEnable() {
      return readEnable;
    }

    public DFEVar getWriteEnable() {
      return writeEnable;
    }

    public DFEVar getRes() {
      return res;
    }

  }

  /** A dense matrix vector multiplier, where the results are accumulated in FMem */
  public static FMemDMVM fmemDMVM(KernelLib owner, int maxBsize,
      DFEVar n, DFEVar nstripes, DFEVar iterations) {
    return new FMemDMVM(owner, n, nstripes, iterations, maxBsize);
  }

  /** A dense matrix vector multiplier, where the results are accumulated in a separate stream.
   *
   * pre: Use this only for large problem sizes, where you are guaranteed that there
   * is sufficient time to write back the data from the current iteration before
   * it is read by the subsequent iterations
   * */
  public static LMemDMVM lmemDMVM(KernelLib owner, int maxBsize,
      DFEVar n, DFEVar nstripes, DFEVar iterations) {
    return new LMemDMVM(owner, n, nstripes, iterations, maxBsize);
  }
}
