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
    private final DFEsmStateValue outValid;
    private final DFEsmStateValue readEnableData, readMaskData, rowFinishedData, rowLengthData;
    private final DFEsmStateValue cycleCounter;
    private final DFEsmStateValue firstReadPosition;
    private final DFEsmStateValue rowsProcessed;

    private final DFEsmStateValue crtPos, toread, iLengthRead;
    private final int inputWidth;
    private final boolean dbg;

    public ParallelCsrReadControl(DFEManager owner, int inputWidth, boolean dbg) {
      super(owner);
      this.inputWidth = inputWidth;
      this.dbg = dbg;

      mode = state.enumerated(Mode.class, Mode.ReadingLength);
      iLength = io.pullInput("length", dfeUInt(32));

      oReadMask    = io.pushOutput("readmask", dfeUInt(inputWidth), 1);
      oReadEnable  = io.pushOutput("readenable", dfeBool(), 1);
      oRowFinished = io.pushOutput("rowFinished", dfeBool(), 1);
      oRowLength   = io.pushOutput("rowLength", dfeUInt(32), 1);
      oNnzCounter  = io.pushOutput("cycleCounter", dfeUInt(32), 1);
      oFirstReadPosition  = io.pushOutput("firstReadPosition", dfeUInt(32), 1);

      cycleCounter = state.value(dfeInt(32), 0);
      rowsProcessed = state.value(dfeInt(32), 0);
      crtPos = state.value(dfeUInt(32), 0);
      firstReadPosition = state.value(dfeUInt(32), 0);
      toread = state.value(dfeUInt(32), 0);
      iLengthRead = state.value(dfeBool(), true);
      readEnableData = state.value(dfeBool(), false);
      rowFinishedData = state.value(dfeBool(), true);
      rowLengthData = state.value(dfeUInt(32), 0);
      outValid = state.value(dfeBool(), false);
      readMaskData = state.value(dfeUInt(inputWidth));
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
      outValid.next <== false;

      iLengthReady.next <== iLengthReady();

      SWITCH (mode) {
        CASE (Mode.ReadingLength) {
          IF (iLengthReady === true) {
            toread.next <== iLength;
            rowLengthData.next <== iLength;
            firstReadPosition.next <== crtPos;
            IF (iLength === 0) {
              makeOutput(fls(), tru(), zero(inputWidth), zero(), zeroI(), crtPos);
              rowsProcessed.next <== rowsProcessed + 1;
            } ELSE {
              mode.next <== Mode.OutputtingCommands;
              iLengthReady.next <== false;
              cycleCounter.next <== -1;
            }
          }
        }
        CASE (Mode.OutputtingCommands) {
          IF (outputNotStall()) {
            rowsProcessed.next <== rowsProcessed + 1;
            outValid.next <== true;
            DFEsmValue canread = min(inputWidth - crtPos, toread);
            readEnableData.next <== crtPos === 0;
            cycleCounter.next <== cycleCounter + 1;
            readMaskData.next <== buildReadMask(canread);

            IF (crtPos + canread >= inputWidth) {
              crtPos.next <== 0;
            } ELSE {
              crtPos.next <== crtPos + canread;
            }

            toread.next <== toread - canread;
            IF (toread - canread === 0) {
              iLengthRead.next <== true;
              rowFinishedData.next <== true;
              mode.next <== Mode.ReadingLength;
            } ELSE {
              iLengthRead.next <== false;
              rowFinishedData.next <== false;
            }
            }
          }
        }
      }

      @Override
      protected void outputFunction() {
        iLength.read <== iLengthReady();

        oReadEnable.valid <== outValid;
        oReadMask.valid <== outValid;
        oRowLength.valid <== outValid;
        oRowFinished.valid <== outValid;
        oNnzCounter.valid <== outValid;
        oFirstReadPosition.valid <== outValid;

        oReadEnable <==readEnableData;
        oReadMask <== readMaskData;
        oRowLength <== rowLengthData;
        oRowFinished <== rowFinishedData;
        oNnzCounter <== cycleCounter.cast(dfeUInt(32));
        oFirstReadPosition <== firstReadPosition;

        if (dbg)
          IF (outValid)
            debug.simPrintf(
                "ReadControl SM -- rowsProcessed %d, iLength %d, readmask: %d, readeenable: %d toread: %d, crtPos: %d, rowLength %d, rowFinished %d cycleCounter %d\n",
                rowsProcessed, iLength, readMaskData, readEnableData, toread, crtPos, rowLengthData, rowFinishedData, cycleCounter);
      }

    void makeOutput(
        DFEsmValue readEnable,
        DFEsmValue rowFinished,
        DFEsmValue readMask,
        DFEsmValue rowLength,
        DFEsmValue cycleCounterP,
        DFEsmValue firstReadPositionP
        )
    {
      outValid.next <== true;

      readEnableData.next <== readEnable;
      rowFinishedData.next <== rowFinished;
      readMaskData.next <== readMask;
      rowLengthData.next <== rowLength;
      cycleCounter.next <== cycleCounterP;
      firstReadPosition.next <== firstReadPositionP;
    }

    DFEsmValue min(DFEsmValue a, DFEsmValue b){
      DFEsmAssignableValue min = assignable.value(dfeUInt(32));
      IF ( a < b) {
        min <== a;
      } ELSE {
        min <== b;
      }
      return min;
    }

    DFEsmValue buildReadMask(DFEsmValue canread) {
      DFEsmAssignableValue pattern = assignable.value(dfeUInt(64));
      pattern <== 0;
      for (long i = 0; i <= inputWidth; i++)
        IF (canread === i)
          pattern <== (1l << i) - 1l;

      DFEsmAssignableValue newReadMask = assignable.value(dfeUInt(inputWidth));
      newReadMask <== 0;
      for (int i = 0; i <= inputWidth; i++)
        IF (crtPos === i)
          newReadMask <== pattern.shiftLeft(i).cast(dfeUInt(inputWidth));

      return newReadMask;
    }

    DFEsmValue fls() {
      return constant.value(false);
    }

    DFEsmValue tru(){
      return constant.value(true);
    }

    DFEsmValue one() {
      return constant.value(dfeUInt(32), 1);
    }

    DFEsmValue zero() {
      return constant.value(dfeUInt(32), 0);
    }

    DFEsmValue zero(int bitWidth) {
      return constant.value(dfeUInt(bitWidth), 0);
    }

    DFEsmValue zeroI() {
      return constant.value(dfeInt(32), 0);
    }


}
