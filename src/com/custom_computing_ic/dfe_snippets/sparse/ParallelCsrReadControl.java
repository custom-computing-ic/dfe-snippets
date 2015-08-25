package com.custom_computing_ic.dfe_snippets.sparse;

import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;
import com.maxeler.maxcompiler.v2.managers.DFEManager;

public class ParallelCsrReadControl extends ManagerStateMachine {

    private final DFEsmPullInput iLength;
    private final DFEsmStateValue iLengthReady;
    private final DFEsmPushOutput oReadMask, oReadEnable;
    private final DFEsmStateValue readEnableOutValid, readMaskOutValid;
    private final DFEsmStateValue readEnableData, readMaskData;

    private final DFEsmStateValue crtPos, toread, iLengthRead;
    private final int inputWidth;

    public ParallelCsrReadControl(DFEManager owner, int inputWidth) {
      super(owner);
      this.inputWidth = inputWidth;

      iLength = io.pullInput("length", dfeUInt(32));

      oReadMask = io.pushOutput("readmask", dfeUInt(inputWidth), 1);
      oReadEnable = io.pushOutput("readenable", dfeBool(), 1);

      crtPos = state.value(dfeUInt(32), 0);
      toread = state.value(dfeUInt(32), 0);
      iLengthRead = state.value(dfeBool(), true);
      readEnableOutValid = state.value(dfeBool(), true);
      readEnableData = state.value(dfeBool(), true);
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
        iLengthRead.next <== false;
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

        readEnableData.next <== crtPos + canread >= inputWidth;

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

      oReadEnable <==readEnableData;
      oReadMask <== readMaskData;

      //IF (readEnableOutValid)
        //debug.simPrintf("Outputting command, readmask: %d, readeenable: %d toread: %d, crtPos: %d\n",
            //readMaskData, readEnableData, toread, crtPos);
    }
}
