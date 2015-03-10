import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;
import com.maxeler.maxcompiler.v2.utils.MathUtils;


class VectorCache extends Kernel {

  protected VectorCache(KernelParameters parameters,
                        int numPipes,
                        int cacheSize,
                        SpmvEngineParams engineParams
                    ) {
    super(parameters);
    int vRomAddressSizeBits = MathUtils.bitsToAddress(cacheSize);

    DFEType FLOAT = dfeFloat(11, 53);
    for (int i = 0; i < numPipes; i++) {
   DFEVar indptr = io.input("indptr_in" + i, dfeInt(32)).cast(dfeUInt(vRomAddressSizeBits)); // col ptr
      int memId = engineParams.getEnableVRomPortSharing()? (i / 2) : i;
      Memory<DFEVar> vRom = mem.alloc(FLOAT, cacheSize);
      vRom.mapToCPU("vRom" + i);
      DFEVar vectorValue = vRom.read(indptr);

      io.output("vector_value_out" + i, vRom.read(indptr), FLOAT);
    }
  }}
