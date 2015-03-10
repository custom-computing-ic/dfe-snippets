/*** Single pipe SpMV Processing element. */

import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;

public class ProcessingElement extends KernelLib {

    private final DFEVar reducedOut;

    protected ProcessingElement(KernelLib owner,
                                int fpL,
                                boolean dbg,
                                DFEVar rowLength,
                                DFEVar rowFinished,
                                DFEVar indptr,
                                DFEVar value,
                                int id,
                                int cacheSize,
                                boolean vRomPortSharing) {
        super(owner);

        DFEType FLOAT = dfeFloat(11, 53);

        // counter is set to 0 when row has finished
        // while counter is less than fpL, we can output results
        DFEVar counterReset = rowFinished.eq(1) | rowFinished.eq(2);

        int memId = vRomPortSharing? (id / 2) : id;
        Memory<DFEVar> vRom = mem.alloc(FLOAT, cacheSize);
        vRom.mapToCPU("vRom" + memId);

        // // --- compute
        Params params = control.count.makeParams(32)
            .withReset(rowFinished.eq(2) | rowFinished.eq(3))
            .withMax(rowLength + 1);
        DFEVar nnzCounter = control.count.makeCounter(params).getCount();

        DFEVar carriedSum = dfeFloat(11, 53).newInstance(this);
        DFEVar newValue = value * vRom.read(indptr);
        DFEVar newSum = newValue + (nnzCounter < fpL ? 0 : carriedSum);
        carriedSum <== stream.offset(newSum, -fpL);

        // First we need to count last no-more-than 16 nonzeros in each row
        // (16 stands for depth of log-add reduce circuit).
        DFEVar firstValidPartialSum = (rowLength + 1 > fpL)? (rowLength + 1 - fpL) : 0;
        DFEVar validPartialSums =  (nnzCounter >= firstValidPartialSum);
        LogAddReduce r = new LogAddReduce(this,
                                          validPartialSums,
                                          counterReset,
                                          newSum, FLOAT, fpL);

        this.reducedOut = r.getOutput();
        optimization.popDSPFactor();

        if (dbg) {
            DFEVar cycleCount = control.count.simpleCounter(32);
            debug.simPrintf("Pipe %d, cycle %d, validpartialSums %d, nnzcounter %d, output %f, enable %d\n",
                            id, cycleCount, validPartialSums, nnzCounter, reducedOut, counterReset);
        }
    }

    DFEVar getReducedOut() {
        return reducedOut;
    }

}
