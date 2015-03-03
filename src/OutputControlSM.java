import com.maxeler.maxcompiler.v2.managers.DFEManager;
import com.maxeler.maxcompiler.v2.statemachine.*;
import com.maxeler.maxcompiler.v2.statemachine.manager.*;

class OutputControlSM extends ManagerStateMachine {

    private static final int numPipes = 2;

    private final DFEsmPullInput[] in = new DFEsmPullInput[numPipes];
    private final DFEsmPushOutput out;

    private final DFEsmStateValue outData, outValid;
    private final DFEsmStateValue inReady[] = new DFEsmStateValue[numPipes];

    private final boolean dbg;

    OutputControlSM(DFEManager owner, boolean dbg) {
        super(owner);

        this.dbg = dbg;

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

        // Generated using gen_sim.py
        IF (inReady[ 0 ]) {
            consume( 0 );
        }
        ELSE IF (inReady[ 1 ]) {
            consume( 1 );
        }
        ELSE IF (inReady[ 2 ]) {
            consume( 2 );
        }
        ELSE IF (inReady[ 3 ]) {
            consume( 3 );
        }
        ELSE IF (inReady[ 4 ]) {
            consume( 4 );
        }
        ELSE IF (inReady[ 5 ]) {
            consume( 5 );
        }
        ELSE IF (inReady[ 6 ]) {
            consume( 6 );
        }
        ELSE IF (inReady[ 7 ]) {
            consume( 7 );
        }
        ELSE IF (inReady[ 8 ]) {
            consume( 8 );
        }
        ELSE IF (inReady[ 9 ]) {
            consume( 9 );
        }
        ELSE IF (inReady[ 10 ]) {
            consume( 10 );
        }
        ELSE IF (inReady[ 11 ]) {
            consume( 11 );
        }
        ELSE IF (inReady[ 12 ]) {
            consume( 12 );
        }
        ELSE IF (inReady[ 13 ]) {
            consume( 13 );
        }
        ELSE IF (inReady[ 14 ]) {
            consume( 14 );
        }
        ELSE IF (inReady[ 15 ]) {
            consume( 15 );
        }
        ELSE IF (inReady[ 16 ]) {
            consume( 16 );
        }
        ELSE IF (inReady[ 17 ]) {
            consume( 17 );
        }
        ELSE IF (inReady[ 18 ]) {
            consume( 18 );
        }
        ELSE IF (inReady[ 19 ]) {
            consume( 19 );
        }
        ELSE IF (inReady[ 20 ]) {
            consume( 20 );
        }
        ELSE IF (inReady[ 21 ]) {
            consume( 21 );
        }
        ELSE IF (inReady[ 22 ]) {
            consume( 22 );
        }
        ELSE IF (inReady[ 23 ]) {
            consume( 23 );
        }
        ELSE IF (inReady[ 24 ]) {
            consume( 24 );
        }
        ELSE IF (inReady[ 25 ]) {
            consume( 25 );
        }
        ELSE IF (inReady[ 26 ]) {
            consume( 26 );
        }
        ELSE IF (inReady[ 27 ]) {
            consume( 27 );
        }
        ELSE IF (inReady[ 28 ]) {
            consume( 28 );
        }
        ELSE IF (inReady[ 29 ]) {
            consume( 29 );
        }
        ELSE IF (inReady[ 30 ]) {
            consume( 30 );
        }
        ELSE IF (inReady[ 31 ]) {
            consume( 31 );
        }


    }

    @Override
    protected void outputFunction() {
        for (int i = 0; i < numPipes; i++) {
            in[i].read <== false;
        }

        IF (inReady( 0 )) {
            in[ 0  ].read <== true;
        }
        ELSE IF (inReady( 1 )) {
            in[ 1 ].read <== true;
        }
        ELSE IF (inReady( 2 )) {
            in[ 2 ].read <== true;
        }
        ELSE IF (inReady( 3 )) {
            in[ 3 ].read <== true;
        }
        ELSE IF (inReady( 4 )) {
            in[ 4 ].read <== true;
        }
        ELSE IF (inReady( 5 )) {
            in[ 5 ].read <== true;
        }
        ELSE IF (inReady( 6 )) {
            in[ 6 ].read <== true;
        }
        ELSE IF (inReady( 7 )) {
            in[ 7 ].read <== true;
        }
        ELSE IF (inReady( 8 )) {
            in[ 8 ].read <== true;
        }
        ELSE IF (inReady( 9 )) {
            in[ 9 ].read <== true;
        }
        ELSE IF (inReady( 10 )) {
            in[ 10 ].read <== true;
        }
        ELSE IF (inReady( 11 )) {
            in[ 11 ].read <== true;
        }
        ELSE IF (inReady( 12 )) {
            in[ 12 ].read <== true;
        }
        ELSE IF (inReady( 13 )) {
            in[ 13 ].read <== true;
        }
        ELSE IF (inReady( 14 )) {
            in[ 14 ].read <== true;
        }
        ELSE IF (inReady( 15 )) {
            in[ 15 ].read <== true;
        }
        ELSE IF (inReady( 16 )) {
            in[ 16 ].read <== true;
        }
        ELSE IF (inReady( 17 )) {
            in[ 17 ].read <== true;
        }
        ELSE IF (inReady( 18 )) {
            in[ 18 ].read <== true;
        }
        ELSE IF (inReady( 19 )) {
            in[ 19 ].read <== true;
        }
        ELSE IF (inReady( 20 )) {
            in[ 20 ].read <== true;
        }
        ELSE IF (inReady( 21 )) {
            in[ 21 ].read <== true;
        }
        ELSE IF (inReady( 22 )) {
            in[ 22 ].read <== true;
        }
        ELSE IF (inReady( 23 )) {
            in[ 23 ].read <== true;
        }
        ELSE IF (inReady( 24 )) {
            in[ 24 ].read <== true;
        }
        ELSE IF (inReady( 25 )) {
            in[ 25 ].read <== true;
        }
        ELSE IF (inReady( 26 )) {
            in[ 26 ].read <== true;
        }
        ELSE IF (inReady( 27 )) {
            in[ 27 ].read <== true;
        }
        ELSE IF (inReady( 28 )) {
            in[ 28 ].read <== true;
        }
        ELSE IF (inReady( 29 )) {
            in[ 29 ].read <== true;
        }
        ELSE IF (inReady( 30 )) {
            in[ 30 ].read <== true;
        }
        ELSE IF (inReady( 31 )) {
            in[ 31 ].read <== true;
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
