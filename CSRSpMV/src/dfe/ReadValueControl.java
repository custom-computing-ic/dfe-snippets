import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;

class ReadValueControl extends Kernel {

    protected ReadValueControl(KernelParameters parameters,
                          int numPipes) {
        super(parameters);
        DFEVector<DFEVar> bcsrv_values  = io.input("bcsrv_values", Types.vdouble(numPipes));
        for (int i = 0; i < numPipes; i++) {
            DFEVar value = bcsrv_values[i];
            io.output("matrix_value" + i, value, dfeFloat(11, 53));
        }
    }

}
