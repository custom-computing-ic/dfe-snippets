import com.maxeler.maxcompiler.v2.managers.DFEManager;
import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;

class OutputControlSM extends ManagerStateMachine {

    private final int numPipes;
    private final DFEsmPullInput[] in;
    private final DFEsmPushOutput out;

    private final DFEsmStateValue outData, outValid;
    private final DFEsmStateValue inReady[];

    private final boolean dbg;

    OutputControlSM(DFEManager owner, int numPipes, boolean dbg) {
        super(owner);

        this.numPipes = numPipes;
        this.dbg = dbg;

        in = new DFEsmPullInput[numPipes];
        inReady = new DFEsmStateValue[numPipes];

        outData  = state.value(dfeUInt(128));
        outValid = state.value(dfeBool(), false);

        for (int i = 0; i < numPipes; i++)
            inReady[i] = state.value(dfeBool(), false);

        // --- inputs
        for (int i = 0; i < numPipes; i++)
            in[i] = io.pullInput("oc_in_" + i, dfeUInt(128));

        // -- outputs
        out = io.pushOutput("b", dfeUInt(128), 1);
    }

    private DFEsmValue inReady(int i) {
        return ~in[i].empty & ~out.stall;
    }

    private void consume(int i) {
        outData.next <== in[i];
        outValid.next <== true;
    }

    @Override
    protected void nextState() {
        for (int i = 0; i < numPipes; i++) {
            inReady[i].next <== inReady(i);
        }

        outValid.next <== false;

        DFEsmAssignableValue prevValue = assignable.value(dfeBool());
        prevValue <== false;
        for (int i = 0; i < numPipes; i++) {
            IF (inReady[i] & ~prevValue) {
                consume(i);
            }
            prevValue <== inReady[i] & ~prevValue;
        }
    }

    @Override
    protected void outputFunction() {
        DFEsmAssignableValue prevValue = assignable.value(dfeBool());
        prevValue <== false;
        for (int i = 0; i < numPipes; i++) {
            in[i].read <== inReady(i) & ~prevValue;
            prevValue <== inReady(i) & ~prevValue;
        }

        out.valid <== outValid;
        out <== outData;

        if (dbg) {
            IF (outValid === true) {
                debug.simPrintf("SM Out: %d\n", outData);
            }
        }
    }
}
