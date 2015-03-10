/*** Single pipe SpMV kernel. */
import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;
import com.maxeler.maxcompiler.v2.utils.MathUtils;


class fpgaNaiveKernel extends Kernel {

    protected fpgaNaiveKernel(KernelParameters parameters,
                              SpmvEngineParams engineParams,
                              int fpL,
                              int cacheSize,
                              int numPipes,
                              boolean dbg,
                              int id) {
        super(parameters);

        optimization.pushPipeliningFactor(engineParams.getPipeliningFactor());
        optimization.pushDSPFactor(1);

        // How many outputs should this pipe write
        DFEVar outputs = io.scalarInput("outputs", dfeUInt(32));
        DFEVar n = io.scalarInput("n", dfeUInt(32));

        int vRomAddressSizeBits = MathUtils.bitsToAddress(cacheSize);

        DFEVar value = io.input("sp_bcsrv_value_" + id, dfeFloat(11, 53));
        DFEVar rowLength = io.input("rowLength_in" + id, dfeUInt(32));
        DFEVar rowFinished = io.input("rowEnd_in" + id, dfeUInt(32));
        DFEVar indptr = io.input("indptr_in" + id, dfeInt(32)).cast(dfeUInt(vRomAddressSizeBits)); // col ptr

        value = rowFinished.eq(3) ? 0 : value;

        ProcessingElement pe = new ProcessingElement(this, fpL, dbg,
                                        rowLength, rowFinished, indptr, value, id,
                                        cacheSize, engineParams.getEnableVRomPortSharing());

        DFEVar rowEmpty = rowFinished.eq(2);
        DFEVar outputEnable = rowFinished.eq(1) | rowEmpty;
        Params params = control.count.makeParams(32)
            .withEnable(outputEnable);
        DFEVar produced = control.count.makeCounter(params).getCount();

        DFEVar outputEnable2 = outputEnable & produced < outputs;

        DFEVar output = rowEmpty ? 0 : pe.getReducedOut();

        DFEVectorType<DFEVar> outType =
            new DFEVectorType<DFEVar>(dfeFloat(11, 53), 2);

        DFEVector<DFEVar> out = outType.newInstance(this);
        DFEVar tag =  (produced + id * n / numPipes).cast(dfeFloat(11, 53));
        out[0] <== tag;
        out[1] <== output;
        io.output("b" + id,
                  out,
                  outType,
                  outputEnable2);
        if (dbg) {
            debug.simPrintf(
                            "Pipe %d value_in %f, rowEnd_in %d, indptr_in %d, Output %f Tag %f rowLength: %d\n",
                            id, value, rowFinished, indptr, output, tag, rowLength);
        }
    }
}
