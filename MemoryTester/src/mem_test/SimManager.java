/* MAXELER MEMORY TESTER
 *
 * < Simulation Manager >
 * For simulation compilation. Instantiate Main Manager.
 *
 * Written by Kit Cheung, Imperial College London */


package mem_test;

import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;
import com.maxeler.maxcompiler.v2.build.EngineParameters;
import com.maxeler.maxcompiler.v2.managers.DFEModel;

public class SimManager {
	public static void main(String[] args) {
		// User parameters (requires hardware reconfig)
		int MAX_STREAMS = 4;				// Number of parallel command / memory
											// streams, decrease to meet timing
		int MEM_STM_WIDTH = 1536;			// Width of the memory interface in DFE,
											// decrease to meet timing
		boolean LMEM_LINEAR_ACCESS = true;	// Use linear access pattern instead of
											// custom memory commands

		EngineParameters engineParams = new EngineParams(args);
		MemoryControllerConfig mem_cfg = new MemoryControllerConfig();
		if (engineParams.getDFEModel() == DFEModel.MAX4848A){
			mem_cfg.setEnableParityMode(true, true, 72, false);
		}

		MainManager m = new MainManager(engineParams, mem_cfg, MAX_STREAMS,
				MEM_STM_WIDTH, LMEM_LINEAR_ACCESS);

		m.build();
	}
}
