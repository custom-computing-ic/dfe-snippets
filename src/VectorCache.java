import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.utils.MathUtils;


class VectorCache extends Kernel {

  protected VectorCache(KernelParameters parameters,
                        int numPipes,
                        int cacheSize,
                        SpmvEngineParams engineParams
                    ) {
    super(parameters);
    int bits = MathUtils.bitsToAddress(cacheSize);

    for (int i = 0; i < numPipes; i++) {
      DFEVar indptr = io.input("indptr_in" + i, dfeInt(32)).cast(dfeUInt(bits));
      Memory<DFEVar> vRom = mem.alloc(Types.dbl(), cacheSize);
      vRom.mapToCPU("vRom" + i);
      DFEVar vectorValue = vRom.read(indptr);
      io.output("vector_value_out" + i, vRom.read(indptr), Types.dbl());
    }
  }}
