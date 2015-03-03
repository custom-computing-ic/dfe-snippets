import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.*;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;
import com.maxeler.maxcompiler.v2.utils.MathUtils;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.KernelMath;
import java.util.*;

/** A multi pipe vector add, subtract kernel. */
class EvalKernel extends Kernel {

    protected EvalKernel(KernelParameters parameters,
                         int numPipes) {
        super(parameters);

        DFEType ioType = dfeFloat(8, 24);
        DFEVectorType<DFEVar> input_t = new DFEVectorType<DFEVar>(ioType, numPipes);
        DFEVectorType<DFEVar> output_t = new DFEVectorType<DFEVar>(ioType, numPipes);

        DFEVector<DFEVar> a = io.input("a", input_t);
        DFEVector<DFEVar> b = io.input("b", input_t);

        io.output("sum", a + b, output_t);
        io.output("diff", a - b, output_t);
    }
}
