package com.custom_computing_ic.dfe_snippets.utils;

import com.maxeler.maxcompiler.v2.kernelcompiler.KernelLib;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEType;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVector;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.composite.DFEVectorType;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.KernelMath;
import com.maxeler.maxcompiler.v2.utils.*;


/***
    This implements a FIFO buffer which enables pushing fixed number of items
    and fetching arbitrary number of items per cycle.
    
    The number of entries to fetch is a run time parameter which varies from 0
    to tupleSize. The maximum number of entries fetchable per cycle and the
    input width is a compile time parameter (tupleSize).

    Inputs and outputs are DFEVector of tupleSize, where output vector carries
    requested number of entries popped from FIFO, and rest of that vector filled
    with zeroes.

    The buffer occasionally issues stall signal (nextPushEnable() == 0)
    to avoid overflow of internal FIFOs and help orchestrating outer
    code. In case of underflow (requesting more entries than internally stored)
    it returns stored entries and zeroes.

    Assumed use case: multi-pipe processing of input stream of length
    not divisible to number of pipes p. At the end of the stream the number
    of values to be read is less then p; this buffer helps not to go
    across the boundary and not to loose/discard values happen to be
    beyond the boundary.

*/

public class FetchSubTuple extends KernelLib
{
    private static final int bufferDepth = 4;

    private DFEVectorType<DFEVar> m_tupleType;
    private DFEVectorType<DFEVar> m_boolTupleType;
    private DFEType m_contentType;

    private int     m_tupleSize;
    private int     m_tupleBitWidth;
    private boolean m_align;

    private DFEVector<DFEVector<DFEVar>> m_buffer;
    // current depth (fill level) at each FIFO component
    private DFEVector<DFEVar>  m_depth;
    private DFEVar m_nextPushEnable;

    /***
        @param   tupleSize      The maximum number of entries to be processed per cycle.
        @param   dataBitWidth   The bitwidth of dataType: necessary for correct initialisation of internal FIFOs.
        @param   dataType       The type of the content stored.
        @param   align          Whether to align the content of sub-tuple to 0-th index at the output (default=false)
    */
    public FetchSubTuple(KernelLib owner, String name, int tupleSize, int dataBitWidth,
                         DFEType dataType, boolean align)
    {
        super(owner);

        m_align            = align;
        m_tupleBitWidth    = MathUtils.bitsToAddress(tupleSize);
        int depthBitWidth = MathUtils.bitsToAddress(bufferDepth);
        m_contentType      = dataType;
        m_tupleSize        = tupleSize;
        m_tupleType =
                new DFEVectorType<DFEVar> (m_contentType, m_tupleSize);
        m_boolTupleType = 
                new DFEVectorType<DFEVar> (dfeBool(), m_tupleSize);

        m_nextPushEnable = dfeBool().newInstance(this);
        m_depth = new DFEVectorType<DFEVar> (dfeUInt(depthBitWidth), m_tupleSize).newInstance(this);

        // 2 dim array: first dim = 1..tupleSise, 2nd dim = 1..bufferDepth
        DFEVectorType<DFEVar> fifo_type = new DFEVectorType<DFEVar>(m_contentType, bufferDepth);
        m_buffer = new DFEVectorType<DFEVector<DFEVar>>(fifo_type, tupleSize).newInstance(this);
    }

    public DFEVar nextPushEnable() { return m_nextPushEnable; }


    /***
        @param  subTupleSize  Number of elements to retrieve. Must be between 0 and tupleSize.
        @param  enable        Boolean: indicates whether inputTuple is requested to be pushed into the buffer.
        @param  inputTuple    Vector of input data. All its tupleSize entries are pushed to the buffer, if return is 1.
        @return               Vector of tupleSize, with only subTupleSize entries retrieved from the buffer.
    */
    public DFEVector<DFEVar> popPush(DFEVar subTupleSize, DFEVar pushEnable, DFEVector<DFEVar> inputTuple)
    {
        DFEType tupleIndexType = null;
        if (m_tupleBitWidth == 0)
        {
            tupleIndexType = dfeUInt(1);
        }
        else
        {
            tupleIndexType = dfeUInt(m_tupleBitWidth);
        }

        // ----------------------------------------------------------------------
        //  Manage state
        // ----------------------------------------------------------------------

        // in case subTupleSize has incompatible bit width
        DFEVar numElements = subTupleSize.cast(tupleIndexType);

        DFEVector<DFEVar> readEnable = m_boolTupleType.newInstance(this);
        DFEVector<DFEVar> mask       = m_boolTupleType.newInstance(this);
        DFEVar shiftLoop             = tupleIndexType.newInstance(this);

        optimization.pushPipeliningFactor(0.0);
            DFEVar shift = control.count.pulse(1)? 0 : stream.offset(shiftLoop,-1);
            shiftLoop <== KernelMath.modulo(shift.cast(dfeUInt(m_tupleBitWidth+1)) + 
                                            numElements.cast(dfeUInt(m_tupleBitWidth+1)), m_tupleSize)
                          .cast(tupleIndexType);
        optimization.popPipeliningFactor();

        for (int i = 0; i < m_tupleSize; i++)
        {
            mask[i] <== (i < numElements);
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
        optimization.pushPipeliningFactor(0.0);
            // all FIFOs have same depth +/-1 - just watching an arbitrary FIFO
            DFEVar pastDepth0 = control.count.pulse(1)? 0 : stream.offset(m_depth[0],-1);
            m_nextPushEnable = pastDepth0 < bufferDepth-3;

            for (int i = 0; i < m_tupleSize; i++)
            {
                DFEVar pastDepth = control.count.pulse(1)? 0 : stream.offset(m_depth[i],-1);
                m_depth[i] <== pushEnable?
                                    (readEnable[i]? pastDepth : (pastDepth+1))
                                   :(readEnable[i]? (pastDepth-1) : pastDepth);
            }

        optimization.popPipeliningFactor();

        // ----------------------------------------------------------------------
        //  Data storage update
        // ----------------------------------------------------------------------

        DFEVector<DFEVar> tuple     = m_tupleType.newInstance(this);

        optimization.pushPipeliningFactor(0.0);
        for (int i = 0; i < m_tupleSize; i++)
        {

            DFEVar pastDepth = control.count.pulse(1)? 0 : stream.offset(m_depth[i],-1);

            DFEVar asIfReadEnable = (pushEnable & pastDepth.eq(0))?
                                               inputTuple[i]
                                             : stream.offset(m_buffer[i][0],-1);
            tuple[i] <== readEnable[i]? asIfReadEnable
                                      : constant.var(m_contentType, 0);
            // shifting those FIFOs that are being read + inserting new entries
            for (int j = 0; j < bufferDepth; j++)
            {
                if (j == bufferDepth-1)
                {
                    m_buffer[i][j] <== (pushEnable & pastDepth.eq(j))? inputTuple[i] : 0;
                }
                else
                {
                    DFEVar asIfReadDisabled = (pushEnable & pastDepth.eq(j))?
                                               inputTuple[i]
                                             : stream.offset(m_buffer[i][j],-1);
                    DFEVar asIfReadEnabled  = (pushEnable & pastDepth.eq(j+1))?
                                               inputTuple[i]
                                             : stream.offset(m_buffer[i][j+1],-1);
                    m_buffer[i][j] <== readEnable[i]? asIfReadEnabled
                                                    : asIfReadDisabled;
                }
            }
        }
        optimization.popPipeliningFactor();
    
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
}
