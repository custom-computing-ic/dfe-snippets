package com.custom_computing_ic.dfe_snippets.sparse;

import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;
import com.maxeler.maxcompiler.v2.managers.DFEManager;

public class ParallelCsrReadControl extends ManagerStateMachine {

    private final DFEsmPullInput iLength;
    private final DFEsmStateValue iLengthReady;
    private final DFEsmPushOutput oReadMask, oReadEnable, oRowFinished, oRowLength, oNnzCounter;
    private final DFEsmStateValue readEnableOutValid, readMaskOutValid;
    private final DFEsmStateValue readEnableData, readMaskData, rowFinishedData, rowLengthData;
    private final DFEsmStateValue nnzCounter;

    private final DFEsmStateValue crtPos, toread, iLengthRead;
    private final int inputWidth;
    private final boolean dbg;

    public ParallelCsrReadControl(DFEManager owner, int inputWidth, boolean dbg) {
      super(owner);
      this.inputWidth = inputWidth;
      this.dbg = dbg;

      iLength = io.pullInput("length", dfeUInt(32));

      oReadMask    = io.pushOutput("readmask", dfeUInt(inputWidth), 1);
      oReadEnable  = io.pushOutput("readenable", dfeBool(), 1);
      oRowFinished = io.pushOutput("rowFinished", dfeBool(), 1);
      oRowLength   = io.pushOutput("rowLength", dfeUInt(32), 1);
      oNnzCounter  = io.pushOutput("nnzCounter", dfeUInt(32), 1);

      nnzCounter = state.value(dfeUInt(32), 0);
      crtPos = state.value(dfeUInt(32), 0);
      toread = state.value(dfeUInt(32), 0);
      iLengthRead = state.value(dfeBool(), true);
      readEnableOutValid = state.value(dfeBool(), false);
      readEnableData = state.value(dfeBool(), false);
      rowFinishedData = state.value(dfeBool(), false);
      rowLengthData = state.value(dfeUInt(32), 0);
      readMaskOutValid = state.value(dfeBool(), false);
      readMaskData = state.value(dfeUInt(inputWidth));
      iLengthReady = state.value(dfeBool(), false);
    }

    DFEsmValue iLengthReady() {
      return iLengthRead & ~iLength.empty & ~oReadEnable.stall & ~oReadMask.stall;
    }

    @Override
    protected void nextState() {
      iLengthReady.next <== iLengthReady();

      readEnableOutValid.next <== false;
      readMaskOutValid.next <== false;

      DFEsmAssignableValue toreadCrt = assignable.value(dfeUInt(32));
      IF (iLengthReady === true) {
        toreadCrt <== iLength;
        nnzCounter.next <== 0;
        IF (iLength !== 0) {
          iLengthRead.next <== false;
          rowLengthData.next <== iLength;
        } ELSE {
          // handle empty row
          rowFinishedData.next <== 1;
          readMaskData.next <== 0;
          rowLengthData.next <== 0;
          readEnableData.next <== 0;
          readMaskOutValid.next <== true;
          readEnableOutValid.next <== true;
        }
      } ELSE {
        toreadCrt <== toread;
      }

      IF (toreadCrt !== 0) {
        readEnableOutValid.next <== true;
        readMaskOutValid.next <== true;

        DFEsmAssignableValue canread = assignable.value(dfeUInt(32));
        IF ( inputWidth - crtPos < toreadCrt) {
          canread <== inputWidth - crtPos;
        } ELSE {
          canread <== toreadCrt;
        }

        readEnableData.next <== crtPos === 0;
        nnzCounter.next <== nnzCounter + canread;

        DFEsmAssignableValue pattern = assignable.value(dfeUInt(64));
        pattern <== 0;
        for (long i = 0; i <= inputWidth; i++)
          IF (canread === i)
            pattern <== (1l << i) - 1l;

        for (int i = 0; i <= inputWidth; i++)
          IF (crtPos === i)
            readMaskData.next <== pattern.shiftLeft(i).cast(dfeUInt(inputWidth));

        // crtPos = (crtPos + canread ) % inputWidth;
        IF (crtPos + canread >= inputWidth) {
          crtPos.next <== 0;
        } ELSE {
          crtPos.next <== crtPos + canread;
        }
        toread.next <== toreadCrt - canread;
        IF (toreadCrt - canread === 0) {
          iLengthRead.next <== true;
          rowFinishedData.next <== 1;
        } ELSE {
          rowFinishedData.next <== 0;
        }
        //debug.simPrintf( "Sm: readEnable: %d, readmask: %d toread %d crtPos %d\n",
        //readEnableData, readMaskData, toread, crtPos);
      }
    }

    @Override
    protected void outputFunction() {
      iLength.read <== iLengthReady();

      oReadEnable.valid <== readEnableOutValid;
      oReadMask.valid <== readMaskOutValid;
      oRowLength.valid <== readMaskOutValid;
      oRowFinished.valid <== readMaskOutValid;
      oNnzCounter.valid <== readMaskOutValid;

      oReadEnable <==readEnableData;
      oReadMask <== readMaskData;
      oRowLength <== rowLengthData;
      oRowFinished <== rowFinishedData;
      oNnzCounter <== nnzCounter;

      if (dbg)
        IF (readEnableOutValid)
          debug.simPrintf(
              "ReadControl SM -- readmask: %d, readeenable: %d toread: %d, crtPos: %d, rowLength %d, rowFinished %d\n",
              readMaskData, readEnableData, toread, crtPos, rowLengthData, rowFinishedData);
    }
}
