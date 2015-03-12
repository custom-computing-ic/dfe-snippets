import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.*;

class Types {

    static public DFEVectorType<DFEVar> vdouble(int size) {
         return new DFEVectorType<DFEVar>(KernelLib.dfeFloat(11, 53), size);
    } 

    static public DFEVectorType<DFEVar> vfloat(int size) {
         return new DFEVectorType<DFEVar>(KernelLib.dfeFloat(8, 24), size);
    } 

    static public DFEVectorType<DFEVar> vint32(int size) {
         return new DFEVectorType<DFEVar>(KernelLib.dfeInt(32), size);
    } 

    static public DFEVectorType<DFEVar> vuint32(int size) {
         return new DFEVectorType<DFEVar>(KernelLib.dfeUInt(32), size);
    } 
}
