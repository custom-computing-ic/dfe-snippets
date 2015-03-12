import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;

class ReadControl extends Kernel {

    protected ReadControl(KernelParameters parameters,
                          int numPipes) {
        super(parameters);

        DFEVector<DFEVar> regInput = io.input("indptr", Types.vuint32(numPipes));

        DFEVar cycleCount = control.count.simpleCounter(32);

        DFEVector<DFEVar> bcsrv_values  = io.input("bcsrv_values", Types.vdouble(numPipes));

        for (int i = 0; i < numPipes; i++) {
            DFEVar value = bcsrv_values[i];
            io.output("rc_bcsrv_value_" + i, value, dfeFloat(11, 53));
            DFEVar input_counts = io.scalarInput("input_count_" + i, dfeUInt(32));
            DFEVar enable = cycleCount < input_counts;
            io.output("readControl_out" + i, regInput[i], dfeUInt(32), enable);
        }
    }

}
