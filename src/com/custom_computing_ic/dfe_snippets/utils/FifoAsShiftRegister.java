package com.custom_computing_ic.dfe_snippets.utils;

import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVector;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVectorType;

/***
    This implements a FIFO buffer of fixed length as shift register.
    Push and Pop operations are performed same time. Data movements
    are done with 1 cycle latency.

    Input value is put to the end of a buffer and will come out
    (return value) only after bufferCapacity enabled cycles.
    The output at first bufferCapacity cycles is undefined.
*/
public class FifoAsShiftRegister extends KernelLib
{
    private DFEVector<DFEVar> buffer;
    private int capacity;
    private DFEType contentType;

    public FifoAsShiftRegister(KernelLib owner, int bufferCapacity, DFEType dataType)
    {
        super(owner);

        capacity = bufferCapacity;
        contentType = dataType;
        buffer =
               new DFEVectorType<DFEVar> (contentType, capacity).newInstance(this);
    }

    public DFEVar popPush(DFEVar enable, DFEVar input)
    {
        DFEVar stored = contentType.newInstance(this);

        optimization.pushPipeliningFactor(0.0);
        {
            // pop last entry
            stored <== enable? stream.offset(buffer[capacity-1],-1)
                             : stream.offset(stored,-1);

            // shift data, overwriting (capacity-1)-th item
            for (int i = 1; i < capacity; i++)
            {
                buffer[i] <== enable? stream.offset(buffer[i-1],-1)
                                      : stream.offset(buffer[i],  -1);
            }
            // push new entry
            buffer[0] <== enable? input : stream.offset(buffer[0],-1);
        }
        optimization.popPipeliningFactor();

        return stored;
    }

}
