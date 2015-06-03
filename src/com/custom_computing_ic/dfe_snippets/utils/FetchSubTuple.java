package com.custom_computing_ic.dfe_snippets.utils;

import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVector;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVectorType;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.KernelMath;
import com.maxeler.maxcompiler.v2.kernelcompiler.SMIO;
import com.maxeler.maxcompiler.v2.utils.*;


/***
    This implements a FIFO buffer which enables fetching arbitrary
    amounts of data per cycle. The maximum number of entries fetchable
    per cycle is a compile time parameter (tupleSize). The number of
    entries to fetch can differ each cycle and vary from 0 to tupleSize.
    The output is a DFEVector with requested number of entries and rest
    of the vector filled with zeroes. Compile time parameter allows
    aligning nonzero entries in the output vector to its 0th index.

    The buffer occasionally issues stall signal (nextPushEnable() == 0)
    to avoid overflow of internal FIFOs and help orchestrating outer
    code.

    Assumed use case: multi-pipe processing of input stream of length
    not divisible to number of pipes p. At the end of the stream the number
    of values to be read is less then p; this buffer helps not to go
    across the boundary and not to loose/discard values happen to be
    beyond the boundary.

*/

public class FetchSubTuple extends KernelLib
{
    // Black magic: don't decrease to avoid VHDL compile errors.
    // Doesn't make the difference for simulation though.
    private static final int bufferDepth = 16;

    private DFEVectorType<DFEVar> m_tupleType;
    private DFEVectorType<DFEVar> m_boolTupleType;
    private DFEType m_contentType;
    private DFEType m_indexType;

    private int     m_tupleSize;
    private int     m_tupleBitWidth;
    private boolean m_align;

    private SMIO[]  m_buffer;

    private DFEVar m_pushAccepted;
    private DFEVar m_nextPushEnable;
    private DFEVar m_popSuccessful;
    private DFEVar m_numElementsStored;

    /***
        @param   inputSize      The maximum number of entries to be processed per cycle.
        @param   dataBitWidth   The bitwidth of dataType: necessary for correct initialisation of internal FIFOs.
        @param   dataType       The type of the content stored.
        @param   align          Boolean indicating whether a sub-tuple needs to be aligned to 0-th index of output vector.
    */
    public FetchSubTuple(KernelLib owner, String name, int tupleSize, int dataBitWidth, DFEType dataType, boolean align)
    {
        super(owner);

        m_align            = align;
        m_tupleBitWidth    = MathUtils.bitsToAddress(tupleSize);
        m_contentType      = dataType;
        m_indexType        = dfeInt(dataBitWidth);
        m_tupleSize        = tupleSize;
        m_tupleType =
                new DFEVectorType<DFEVar> (m_contentType, m_tupleSize);
        m_boolTupleType = 
                new DFEVectorType<DFEVar> (dfeBool(), m_tupleSize);

        m_nextPushEnable    = dfeBool().newInstance(this);
        m_pushAccepted      = dfeBool().newInstance(this);
        m_numElementsStored = m_indexType.newInstance(this);
        m_popSuccessful     = dfeBool().newInstance(this);

        m_buffer = new SMIO[tupleSize];
        for (int i = 0; i < tupleSize; i++)
        {
            m_buffer[i] = addStateMachine("Sm" + name + "" + i, new FifoWrapperSM(this, dataBitWidth, bufferDepth));
        }
    }

    public DFEVar nextPushEnable() { return m_nextPushEnable; }
    public DFEVar isPopSuccessful(){ return m_popSuccessful;  }


    /***
        @param   enable      Boolean: indicates whether inputTuple is requested to be pushed into the buffer.
        @param   inputTuple  Vector of input data. All its tupleSize entries are pushed to the buffer, if return is 1.
        @return              Returns 0 if push is unsuccessful and 1 otherwise.
    */
    public DFEVar push(DFEVar pushEnable, DFEVector<DFEVar> inputTuple)
    {
        optimization.pushPipeliningFactor(0.0);
            // Check for space being available in the buffer
            DFEVar pastNumElements = control.count.pulse(1)? 0 : stream.offset(m_numElementsStored,-1);
            m_pushAccepted <== pushEnable & (pastNumElements < (bufferDepth-1)*m_tupleSize);
            for (int i = 0; i < m_tupleSize; i++)
            {
                m_buffer[i].connectInput("dataIn", m_indexType.unpack(inputTuple[i].pack()) );
                m_buffer[i].connectInput("writeEnable", m_pushAccepted);
            }
        optimization.popPipeliningFactor();
        return m_pushAccepted;
    }

    /***
        @param  subTupleSize  Number of elements to retrieve. Must be between 0 and tupleSize.
        @return               Vector of tupleSize, with only subTupleSize entries retrieved from the buffer.
    */
    public DFEVector<DFEVar> pop(DFEVar subTupleSize)
    {
        // in case subTupleSize has incompatible bit width with m_indexType
        DFEVar numElements = subTupleSize.cast(m_indexType);

        // ----------------------------------------------------------------------
        //  Manage state
        // ----------------------------------------------------------------------

        DFEType tupleIndexType = null;
        if (m_tupleBitWidth == 0)
        {
            tupleIndexType = dfeUInt(1);
        }
        else
        {
            tupleIndexType = dfeUInt(m_tupleBitWidth);
        }

        DFEVector<DFEVar> readEnable = m_boolTupleType.newInstance(this);
        DFEVector<DFEVar> mask       = m_boolTupleType.newInstance(this);
        DFEVar shiftLoop             = tupleIndexType.newInstance(this);


        optimization.pushPipeliningFactor(0.0);
            DFEVar incomingData = m_pushAccepted? constant.var(m_indexType,m_tupleSize) : constant.var(m_indexType,0);

            DFEVar shift = control.count.pulse(1)? 0 : stream.offset(shiftLoop,-1);
            DFEVar pastNumElements = control.count.pulse(1)? 0 : stream.offset(m_numElementsStored,-1);

            // do we have enough data to pop?
            DFEVar enoughData = (incomingData + pastNumElements >= numElements);
            DFEVar fetchSize  = (enoughData)? numElements : 0;

            // assuming fetchSize has dataBitWidth bits
            shiftLoop  <==  KernelMath.modulo(shift.cast(m_indexType) + fetchSize, m_tupleSize).cast(tupleIndexType);

            m_numElementsStored <== pastNumElements + incomingData - fetchSize;
            m_nextPushEnable <== (m_numElementsStored < (bufferDepth-1)*m_tupleSize);
        optimization.popPipeliningFactor();

        for (int i = 0; i < m_tupleSize; i++)
        {
            mask[i] <== (i < fetchSize);
        }
        readEnable = null;
        if (m_tupleBitWidth == 0)
        {
            readEnable = mask;
        }
        else
        {
            readEnable = mask.rotateElementsLeft(shift);
        }

        // ----------------------------------------------------------------------
        //  Data storage update
        // ----------------------------------------------------------------------

        DFEVector<DFEVar> dataValid = m_boolTupleType.newInstance(this);
        DFEVector<DFEVar> tuple     = m_tupleType.newInstance(this);

        for (int i = 0; i < m_tupleSize; i++)
        {
            m_buffer[i].connectInput("readEnable", readEnable[i]);
            DFEVar fifoOut   = stream.offset(m_buffer[i].getOutput("dataOut"),1);
            DFEVar fifoValid = stream.offset(m_buffer[i].getOutput("outValid"),1);

            dataValid[i] <== fifoValid & readEnable[i];
            DFEVar dataOut = m_contentType.unpack(fifoOut.pack());

            tuple[i] <== dataValid[i]? dataOut : constant.var(m_contentType, 0);
        }
        m_popSuccessful <== logicORReductionTree(dataValid, 0, m_tupleSize-1);

        // align tuple values so that they start from index 0
        if (m_align)
        {
            return tuple.rotateElementsRight(shift);
        }
        else
        {
            return tuple;
        }
    }

    // Recursively produce binary reduction tree with logical OR operation.
    private DFEVar logicORReductionTree(DFEVector<DFEVar> src, int left, int right)
    {
        if (left == right) return src[left];
        int middle = left + (right - left)/2;
        return logicORReductionTree(src, left, middle) | logicORReductionTree(src, middle + 1, right);
    }

}
