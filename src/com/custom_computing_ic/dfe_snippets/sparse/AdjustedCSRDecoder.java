package com.custom_computing_ic.dfe_snippets.sparse;

import com.maxeler.maxcompiler.v2.managers.DFEManager;
import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;


// SM to decode the Adjusted CSR format:
// (length) rowIndptr, values
public class AdjustedCSRDecoder extends ManagerStateMachine {

    private enum Mode {
        ReadRowLength, // first entry ever processed
        ReadRow,
        OutputZero,
    };

    private final DFEsmStateEnum<Mode> mode;

    private final DFEsmPullInput iIndptr;
    private final DFEsmInput iItemCount; // how many items to produce

    private final DFEsmPushOutput oIndptr, oRowEnd, oRowLength;

    private final DFEsmStateValue counter, rowLength;
    private final DFEsmStateValue indptrReady, indptrData;
    private final DFEsmStateValue indptrOutValid, rowEndValid, rowLengthValid;
    private final DFEsmStateValue rowEnd;
    private final DFEsmStateValue itemCount;

    private final int id;

    //    private final DFEsmStateValue sCycleCounter = state.value(dfeUInt(64), 0);
    private boolean dbg = false;

    public AdjustedCSRDecoder(DFEManager owner, boolean dbg, int id) {
        super(owner);
        this.dbg = dbg;
        this.id = id;
        mode = state.enumerated(Mode.class, Mode.ReadRowLength);

        counter = state.value(dfeUInt(32), 0);

        rowLength = state.value(dfeUInt(32));
        rowEnd = state.value(dfeUInt(32));
        itemCount = state.value(dfeUInt(32), 0);

        indptrReady = state.value(dfeBool(), false);
        indptrData  = state.value(dfeUInt(32));

        indptrOutValid = state.value(dfeBool(), false);
        rowEndValid = state.value(dfeBool(), false);
        rowLengthValid = state.value(dfeBool(), false);

        iItemCount = io.scalarInput("output_count", dfeUInt(32));

        // --- inputs
        iIndptr = io.pullInput("indptr", dfeUInt(32));
        // TODO must discard first value of column pointer

        // -- outputs
        oIndptr = io.pushOutput("indptr_out", dfeUInt(32), 1);
        oRowEnd = io.pushOutput("rowEnd_out", dfeUInt(32), 1);
        oRowLength = io.pushOutput("rowLength_out", dfeUInt(32), 1);
    }


    private DFEsmValue indptrReady() {
        return ~iIndptr.empty & ~oIndptr.stall & ~oRowEnd.stall & ~oRowLength.stall;
    }

    @Override
    protected void nextState() {
        indptrReady.next <== indptrReady();

        rowEndValid.next <== false;
        indptrOutValid.next <== false;
        rowLengthValid.next <== false;

        IF (indptrReady === true) {

            rowLengthValid.next <== true;
            rowEndValid.next <== true;
            indptrOutValid.next <== true;

            SWITCH (mode) {
                CASE (Mode.ReadRowLength) {
                    IF (iIndptr === 0) {
                        // empty row
                        rowEnd.next <== 2;
                        indptrData.next <== 0;
                        itemCount.next <== itemCount + 1;
                    } ELSE {
                        rowLength.next <== iIndptr;
                        mode.next      <== Mode.ReadRow;
                        rowEnd.next <== 3;
                        indptrData.next <== 0;
                    }
                }
                CASE (Mode.ReadRow) {
                    // output, save new length
                    itemCount.next <== itemCount + 1;
                    IF (counter < rowLength - 1) {
                        rowEnd.next <== 0;
                        counter.next <== counter + 1;
                    } ELSE {
                        rowEnd.next <== 1;
                        mode.next <== Mode.ReadRowLength;
                        counter.next <== 0;
                    }
                    indptrData.next <== iIndptr;
                }
            }
        }

        SWITCH (mode) {
            CASE (Mode.OutputZero) {
                IF (~oIndptr.stall & ~oRowEnd.stall & ~oRowLength.stall) {
                    rowLengthValid.next <== true;
                    rowLength.next <== 0;
                    rowEndValid.next <== true;
                    rowEnd.next <== 2;
                    indptrOutValid.next <== true;
                    indptrData.next <== 0;
                }

            }
            OTHERWISE {
                IF (itemCount !== 0 & itemCount === iItemCount) {
                    mode.next <== Mode.OutputZero;
                }
            }
        }
    }

    @Override
    protected void outputFunction() {
        iIndptr.read <== indptrReady();

        oIndptr.valid <== indptrOutValid;
        oRowEnd.valid <== rowEndValid;
        oRowLength.valid <== rowLengthValid;

        oIndptr <== indptrData;
        oRowEnd <== rowEnd;
        oRowLength <== rowLength;

        if (dbg)
            IF (indptrOutValid === true)
                debug.simPrintf("SM %d - counter: %d, rowLength: %d, indptr: %d rowEnd: %d itemCount %d expItemCount %d\n",
                                id, counter, rowLength, iIndptr, rowEnd, itemCount, iItemCount);
    }
}
