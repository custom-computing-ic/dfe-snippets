package com.custom_computing_ic.dfe_snippets.sparse;

import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;
import com.maxeler.maxcompiler.v2.managers.DFEManager;

import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;
import com.maxeler.maxcompiler.v2.managers.DFEManager;

public class ParallelCsrReadControl extends ManagerStateMachine {

  private enum Mode {
      ReadingLength,
      OutputtingCommands
    }

  private final DFEsmStateEnum<Mode> mode;

    private final DFEsmPullInput iLength;
    private final DFEsmStateValue iLengthReady;
    private final DFEsmPushOutput oReadMask, oReadEnable, oRowFinished, oRowLength, oNnzCounter, oFirstReadPosition;
    private final DFEsmStateValue readMaskOutValid;
    private final DFEsmStateValue readEnableData, readMaskData, rowFinishedData, rowLengthData;
    private final DFEsmStateValue cycleCounter;
    private final DFEsmStateValue firstReadPosition;

    private final DFEsmStateValue crtPos, toread, iLengthRead;
    private final int inputWidth;
    private final boolean dbg;

    public ParallelCsrReadControl(DFEManager owner, int inputWidth, boolean dbg) {
      super(owner);
      this.inputWidth = inputWidth;
      this.dbg = dbg;

      mode = state.enumerated(Mode.class, Mode.ReadingLength);
      iLength = io.pullInput("length", dfeUInt(32));

      oReadMask    = io.pushOutput("readmask", dfeUInt(32), 1);
      oReadEnable  = io.pushOutput("readenable", dfeUInt(32), 1);
      oRowFinished = io.pushOutput("rowFinished", dfeUInt(32), 1);
      oRowLength   = io.pushOutput("rowLength", dfeUInt(32), 1);
      oNnzCounter  = io.pushOutput("cycleCounter", dfeUInt(32), 1);
      oFirstReadPosition  = io.pushOutput("firstReadPosition", dfeUInt(32), 1);

      cycleCounter = state.value(dfeInt(32), 0);
      crtPos = state.value(dfeUInt(32), 0);
      firstReadPosition = state.value(dfeUInt(32), 0);
      toread = state.value(dfeUInt(32), 0);
      iLengthRead = state.value(dfeBool(), true);
      readEnableData = state.value(dfeUInt(32), false);
      rowFinishedData = state.value(dfeUInt(32), true);
      rowLengthData = state.value(dfeUInt(32), 0);
      readMaskOutValid = state.value(dfeBool(), false);
      readMaskData = state.value(dfeUInt(32));
      iLengthReady = state.value(dfeBool(), false);
    }

    DFEsmValue outputNotStall() {
      return ~oReadEnable.stall & ~oReadMask.stall
        & ~oRowFinished.stall & ~oRowLength.stall
        & ~oNnzCounter.stall & ~oFirstReadPosition.stall;
    }

    DFEsmValue iLengthReady() {
      return ~iLength.empty & outputNotStall() & mode === Mode.ReadingLength;
    }

    @Override
    protected void nextState() {
      readMaskOutValid.next <== false;
      iLengthReady.next <== iLengthReady();

      SWITCH (mode) {
        CASE (Mode.ReadingLength) {
          IF (iLengthReady === true) {
            toread.next <== iLength;
            rowLengthData.next <== iLength;
            IF (iLength === 0) {
              rowFinishedData.next <== 1;
              readEnableData.next <== 0;
              readMaskData.next <== 0;
              rowLengthData.next <== 0;
              cycleCounter.next <== 0;
              firstReadPosition.next <== crtPos;
              readMaskOutValid.next <== true;
              //debug.simPrintf("Empty row \n");
            } ELSE {
              mode.next <== Mode.OutputtingCommands;
              iLengthReady.next <== false;
              cycleCounter.next <== -1;
              //debug.simPrintf("Reading input %d\n", iLength);
            }
          }
        }
        CASE (Mode.OutputtingCommands) {
          IF (outputNotStall()) {
            readMaskOutValid.next <== true;

            DFEsmAssignableValue canread = assignable.value(dfeUInt(32));
            IF ( inputWidth - crtPos < toread) {
              canread <== inputWidth - crtPos;
            } ELSE {
              canread <== toread;
            }

            IF (crtPos === 0) {
              readEnableData.next <== 1;
            } ELSE {
              readEnableData.next <== 0;
            }

            cycleCounter.next <== cycleCounter + 1;

            DFEsmAssignableValue pattern = assignable.value(dfeUInt(64));
            pattern <== 0;
            for (long i = 0; i <= inputWidth; i++)
              IF (canread === i)
                pattern <== (1l << i) - 1l;

            for (int i = 0; i <= inputWidth; i++)
              IF (crtPos === i)
                readMaskData.next <== pattern.shiftLeft(i).cast(dfeUInt(32));

            // crtPos = (crtPos + canread ) % inputWidth;
            IF (crtPos + canread >= inputWidth) {
              crtPos.next <== 0;
            } ELSE {
              crtPos.next <== crtPos + canread;
            }

            //debug.simPrintf(
                //"Next State -- readMaskOutValid %d, canread %d toread %d readMask %d pattern %d cycleCounter %d\n",
                //readMaskOutValid, canread, toread, readMaskData, pattern, cycleCounter);

            toread.next <== toread - canread;
            IF (toread - canread === 0) {
              iLengthRead.next <== true;
              rowFinishedData.next <== 1;
              mode.next <== Mode.ReadingLength;
              // debug.simPrintf("iLengthRead.next = true ");
            } ELSE {
              iLengthRead.next <== false;
              rowFinishedData.next <== 0;
              //  debug.simPrintf("iLengthRead.next = false ");
            }
            //debug.simPrintf( "Sm: iLengthRead %d outEnable: %d, readEnable: %d, readmask: %d toread %d crtPos %d\n",
                //iLengthRead, readMaskOutValid, readEnableData, readMaskData, toread, crtPos);
          }
        }
      }
    }

    @Override
    protected void outputFunction() {
      iLength.read <== iLengthReady();

      oReadEnable.valid <== readMaskOutValid;
      oReadMask.valid <== readMaskOutValid;
      oRowLength.valid <== readMaskOutValid;
      oRowFinished.valid <== readMaskOutValid;
      oNnzCounter.valid <== readMaskOutValid;
      oFirstReadPosition.valid <== readMaskOutValid;

      oReadEnable <==readEnableData;
      oReadMask <== readMaskData;
      oRowLength <== rowLengthData;
      oRowFinished <== rowFinishedData;
      oNnzCounter <== cycleCounter.cast(dfeUInt(32));
      oFirstReadPosition <== firstReadPosition;

      if (dbg)
        IF (readMaskOutValid)
          debug.simPrintf(
              "ReadControl SM -- iLength %d, readmask: %d, readeenable: %d toread: %d, crtPos: %d, rowLength %d, rowFinished %d cycleCounter %d\n",
              iLength, readMaskData, readEnableData, toread, crtPos, rowLengthData, rowFinishedData, cycleCounter);
    }
}
