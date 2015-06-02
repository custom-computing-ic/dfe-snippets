package com.custom_computing_ic.dfe_snippets.utils;

import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVector;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;

import java.util.List;

public class Reductions {

    public static DFEVar reduce(List<DFEVar> vector) {
      if (vector.size() == 1)
        return vector.get(0);
      return
        reduce(vector.subList(0, vector.size() / 2)) +
        reduce(vector.subList(vector.size() / 2, vector.size()));
    }

}
