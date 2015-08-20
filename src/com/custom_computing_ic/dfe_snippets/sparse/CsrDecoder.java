package com.custom_computing_ic.dfe_snippets.sparse;

import com.maxeler.maxcompiler.v2.managers.DFEManager;
import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;

public class CsrDecoder extends ManagerStateMachine {

    private enum Mode {
        ReadRowLength, // first entry ever processed
        ReadRow,
    };

    private final DFEsmStateEnum<Mode> mode;

    private final DFEsmPullInput iIndptr;
    private final DFEsmPullInput iColptr; // containing the indices
    private final DFEsmInput iRowCount;

    private final DFEsmPushOutput oRowLength;
    private final DFEsmPushOutput oIndptr;

    private final DFEsmStateValue prevData, rowLength, indptrData;
    private final DFEsmStateValue colptrReady, indptrReady;
    private final DFEsmStateValue rowLengthValid, indptrOutValid;
    private final DFEsmStateValue counter;

    private final DFEsmStateValue iIndptrRead, iColptrRead;

    private final int id;

    //    private final DFEsmStateValue sCycleCounter = state.value(dfeUInt(64), 0);
    private boolean dbg = false;

    public CsrDecoder(DFEManager owner) {
        super(owner);
        this.dbg = false;
        this.id = 1;
        mode = state.enumerated(Mode.class, Mode.ReadRowLength);

        counter = state.value(dfeUInt(32), 0);
        // rowEnd = state.value(dfeUInt(32));
        rowLength = state.value(dfeUInt(32));
        prevData = state.value(dfeUInt(32), 0);

        indptrReady = state.value(dfeBool(), false);
        colptrReady = state.value(dfeBool(), false);
        indptrData  = state.value(dfeUInt(32));

        iColptrRead = state.value(dfeBool(), true);
        iIndptrRead = state.value(dfeBool(), false);

        indptrOutValid = state.value(dfeBool(), false);
        //rowEndValid = state.value(dfeBool(), false);
        rowLengthValid = state.value(dfeBool(), false);

        // --- inputs
        iIndptr = io.pullInput("indptr", dfeUInt(32));
        iColptr = io.pullInput("colptr", dfeUInt(32));
        iRowCount = io.scalarInput("nrows", dfeUInt(32));
        // TODO must discard first value of column pointer

        // -- outputs
        oIndptr = io.pushOutput("indptr_out", dfeUInt(32), 1);
        //oRowEnd = io.pushOutput("rowEnd_out", dfeUInt(32), 1);
        oRowLength = io.pushOutput("rowLength_out", dfeUInt(32), 1);
    }


    private DFEsmValue indptrReady() {
        return ~iIndptr.empty & ~oIndptr.stall;
    }

    private DFEsmValue colptrReady() {
        return ~iColptr.empty & ~oRowLength.stall;
    }

    @Override
    protected void nextState() {
      colptrReady.next <== colptrReady() & iColptrRead;
      indptrReady.next <== indptrReady() & iIndptrRead;

      rowLengthValid.next <== false;
      indptrOutValid.next <== false;

      SWITCH(mode){
        CASE (Mode.ReadRowLength) {
          IF (colptrReady === true) {
            rowLength.next <== iColptr - prevData;
            counter.next <== iColptr - prevData;
            prevData.next <== iColptr;
            rowLengthValid.next <== true;
            mode.next <== Mode.ReadRow;
            iIndptrRead.next <== true;
            iColptrRead.next <== false;
          }
        }
        CASE (Mode.ReadRow) {
          IF (indptrReady === true) {
            counter.next <== counter - 1;
            iIndptrRead.next <== true;
            IF (counter - 1 === 0) {
              mode.next <== Mode.ReadRowLength;
              iIndptrRead.next <== false;
              iColptrRead.next <== true;
            }
            indptrData.next <== iIndptr;
            indptrOutValid.next <== true;
          }
        }
      }
    }

    @Override
    protected void outputFunction() {
        iIndptr.read <== indptrReady() & iIndptrRead;
        iColptr.read <== colptrReady() & iColptrRead;

        oIndptr.valid <== indptrOutValid;
        oRowLength.valid <== rowLengthValid;

        oIndptr <== indptrData;
        oRowLength <== counter;

        IF (indptrOutValid)
          debug.simPrintf(
              "SM %d - counter: %d, rowLength: %d, indptr: %d prevData: %d\n",
              id, counter, rowLength, iIndptr, prevData);
    }
}
