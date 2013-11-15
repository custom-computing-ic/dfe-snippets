/* MAXELER MEMORY TESTER
 *
 * < Route Kernel >
 * Control the routing of data from LMEM (DRAM) to host through PCI or back to LMEM
 *
 * Written by Kit Cheung, Imperial College London */


package mem_test;

import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;

import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;

public class RouteKernel extends Kernel {
	public RouteKernel (KernelParameters parameters, int max_streams, int mem_stm_width) {
		super(parameters);
		DFEVar mem_to_mem = io.scalarInput("mem_to_mem", dfeBool());
		DFEVar mem_to_host = io.scalarInput("mem_to_host", dfeBool());
		DFEVar ticks = io.scalarInput("ticks", dfeUInt(64));

		int mem_stream_bits = (int)Math.ceil(Math.log(max_streams) / Math.log(2));
		DFEVar mem_streams = io.scalarInput("mem_streams", dfeUInt(mem_stream_bits));

		DFEVar cycle = control.count.simpleCounter(64);
		DFEVar dram_processed = constant.var(dfeRawBits(mem_stm_width), 0);
		for (int i = 0; i < max_streams; i++){
			DFEVar from_dram = io.input("from_dram"+i, dfeRawBits(mem_stm_width), mem_streams >= i);
			// Simple operation to avoid hardware optimize away some logics
			dram_processed = from_dram.slice(0, mem_stm_width-1).cat(constant.var(dfeBool(), 1));

			io.output("to_dram"+i, dram_processed, dfeRawBits(mem_stm_width), mem_to_mem & mem_streams >= i);
		
			debug.printf("[Ctrl:%d] ram_in:%d  ram_out:%d\n", cycle, from_dram, dram_processed);
		}
		io.output("to_host", dram_processed, dfeRawBits(mem_stm_width), mem_to_host | cycle === ticks-1);
	}
}
