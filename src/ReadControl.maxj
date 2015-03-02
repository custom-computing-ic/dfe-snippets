import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;

class ReadControl extends Kernel {

    protected ReadControl(KernelParameters parameters,
                          int numPipes) {
        super(parameters);

        // -- Read indptr values
        DFEVectorType<DFEVar> mainVectorType =
            new DFEVectorType<DFEVar>(dfeUInt(32), numPipes);

        DFEVector<DFEVar> regInput = io.input("indptr", mainVectorType);

        DFEVar cycleCount = control.count.simpleCounter(32);

        for (int i = 0; i < numPipes; i++) {
            DFEVar input_counts = io.scalarInput("input_count_" + i, dfeUInt(32));
            DFEVar enable = cycleCount < input_counts;
            io.output("readControl_out" + i, regInput[i], dfeUInt(32), enable);
        }
    }

}
