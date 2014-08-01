/* MAXELER MEMORY TESTER
 *
 * < DFE Manager >
 * For hardware compilation. Calls a number of hardware configuration methods and
 * instantiate Main Manager.
 * You can change the parameters (MAX_STREAMS, MEM_STM_WIDTH, LMEM_LINEAR_ACCESS, 
 * QUARTER_RATE, OPTIMIZATION) to test various hardware configurations.
 *
 * Written by Kit Cheung, Imperial College London */


package mem_test;

import com.maxeler.maxcompiler.v2.managers.BuildConfig;
import com.maxeler.maxcompiler.v2.build.EngineParameters;
import com.maxeler.maxcompiler.v2.managers.DFEModel;
import com.maxeler.maxcompiler.v2.managers.custom.CustomManager.LMemFrequency;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;

public class DFEManager {
	public static void main(String[] args) {
		// User parameters (requires hardware reconfig)
		int MAX_STREAMS = 4;				// Number of parallel command / memory streams
											// available, decrease to meet timing
		int MEM_STM_WIDTH = 1536;			// Width of the memory interface in DFE,
											// decrease to meet timing
		boolean LMEM_LINEAR_ACCESS = false;	// Use linear access pattern instead of
											// custom memory commands
		boolean QUARTER_RATE = false;		// set to true to enable memory quarter rate mode,
											// only available in MAX4 MAIA, may lower clock
		boolean OPTIMIZATION = false;		// Enable a few configuration options


		EngineParameters engineParams = new EngineParams(args);
		MemoryControllerConfig mem_cfg = new MemoryControllerConfig();

		// create manager with board Model and ID
		if (engineParams.getDFEModel() == DFEModel.MAX4848A){
			mem_cfg.setEnableParityMode(true, true, 72, false);
			if (QUARTER_RATE)
				mem_cfg.setMAX4qMode(true);
			if (OPTIMIZATION)
				mem_cfg.setDataReadFIFOExtraPipelineRegInFabric(true);
		}
		MainManager m = new MainManager(engineParams, mem_cfg, MAX_STREAMS,
				MEM_STM_WIDTH, LMEM_LINEAR_ACCESS);
		// create build configuration
		BuildConfig c = new BuildConfig(BuildConfig.Level.FULL_BUILD);

		// configure the manager depending if basic testing is required
		if (engineParams.getDFEModel() == DFEModel.MAX4848A){
			m.config.setDefaultStreamClockFrequency(150);   // MHZ, default 100
			if (QUARTER_RATE)
				m.config.setOnCardMemoryFrequency(LMemFrequency.MAX4MAIA_800);
			else 
				m.config.setOnCardMemoryFrequency(LMemFrequency.MAX4MAIA_533);
		}
		else {
			m.config.setDefaultStreamClockFrequency(120);   // MHZ, default 100
			m.config.setOnCardMemoryFrequency(LMemFrequency.MAX3_400);
		}

		// add an additional register to the data in the FPGA fabric
		if (OPTIMIZATION)
			m.config.setEnableAddressGeneratorsInSlowClock(true);
		//c.setEnableTimingAnalysis(true);            // generate timing report
		//m.config.setEnablePCIExpressFastClock(true);
		//m.config.setNumberOfPCIExpressLanes(8);
		c.setBuildEffort(BuildConfig.Effort.HIGH);  // LOW,MEDIUM,HIGH,VERY_HIGH


		// configure how the FPGA design is built
		if (engineParams.getDFEModel() == DFEModel.MAX4848A){
			c.setMPPRCostTableSearchRange(1, 3);
			c.setMPPRParallelism(3);
		}
		else {
			c.setMPPRCostTableSearchRange(1, 4);        // set to enable MPPR
			c.setMPPRParallelism(4);                    // use 4 CPU threads
		}

		// build it!
		m.setBuildConfig(c);
		m.build();
	}
}

