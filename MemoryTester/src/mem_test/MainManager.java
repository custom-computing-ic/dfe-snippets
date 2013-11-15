/* MAXELER MEMORY TESTER
 *
 * < Main Manager >
 * Routing of signals from/to FPGA, host and LMEM.
 *
 * Written by Kit Cheung, Imperial College London */


package mem_test;

import com.maxeler.maxcompiler.v2.build.EngineParameters;
import com.maxeler.maxcompiler.v2.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v2.managers.custom.DFELink;
import com.maxeler.maxcompiler.v2.managers.custom.blocks.KernelBlock;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControlGroup.MemoryAccessPattern;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class MainManager extends CustomManager {
	MainManager(EngineParameters engineParams, MemoryControllerConfig mem_cfg, int MAX_STREAMS, int MEM_STM_WIDTH, boolean LMEM_LINEAR_ACCESS) {
		super(engineParams);

		config.setMemoryControllerConfig(mem_cfg);
		
		// Instantiate processing kernel
		KernelBlock k_ctrl = addKernel(new RouteKernel(makeKernelParameters("RouteKernel"), MAX_STREAMS, MEM_STM_WIDTH));
		KernelBlock k_cmd = addKernel(new CmdKernel(makeKernelParameters("CmdKernel"), MAX_STREAMS, LMEM_LINEAR_ACCESS));

		// Setup to/from DRAM
		DFELink from_dram, to_dram;
		for (int i = 0; i < MAX_STREAMS; i++){
			if (LMEM_LINEAR_ACCESS){
				// linear
				from_dram = addStreamFromOnCardMemory("from_dram"+i, MemoryAccessPattern.LINEAR_1D);
				to_dram = addStreamToOnCardMemory("to_dram"+i, MemoryAccessPattern.LINEAR_1D);
			}
			else {
				// custom command
				from_dram = addStreamFromOnCardMemory("from_dram"+i, k_cmd.getOutput("from_dram_cmd"+i));
				to_dram = addStreamToOnCardMemory("to_dram"+i, k_cmd.getOutput("to_dram_cmd"+i));
			}

			k_ctrl.getInput("from_dram"+i) <== from_dram;
			to_dram <== k_ctrl.getOutput("to_dram"+i);
		}


		// to host
		DFELink to_host = addStreamToCPU("to_host");
		to_host <== k_ctrl.getOutput("to_host");


		// Write config parameters to file for host code to read config
		try {
			int lmem_linear_int = LMEM_LINEAR_ACCESS? 1:0;
			String content = 
				Integer.toString(MAX_STREAMS) + " " +
				Integer.toString(MEM_STM_WIDTH) + " " +
				Integer.toString(lmem_linear_int);

			File file = new File(engineParams.getBuildName() + "/config.txt");
			// if file doesnt exists, then create it
			if (!file.exists()) {
				file.createNewFile();
			}

			FileWriter fw = new FileWriter(file.getAbsoluteFile());
			BufferedWriter bw = new BufferedWriter(fw);
			bw.write(content);
			bw.close();

		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
