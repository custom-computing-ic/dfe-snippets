/* MAXELER MEMORY TESTER
 *
 * < Command Kernel >
 * Issue custom memory command to LMEM (DRAM)
 *
 * Written by Kit Cheung, Imperial College London */


package mem_test;

import com.maxeler.maxcompiler.v2.kernelcompiler.Kernel;
import com.maxeler.maxcompiler.v2.kernelcompiler.KernelParameters;
import com.maxeler.maxcompiler.v2.kernelcompiler.types.base.DFEVar;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.LMemCommandStream;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.Counter;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.core.Count.WrapMode;
import com.maxeler.maxcompiler.v2.kernelcompiler.stdlib.memory.Memory;

public class CmdKernel extends Kernel {
	public CmdKernel(KernelParameters parameters, int max_streams, boolean lmem_linear_access) {
		super(parameters);

		// a total number of dram_bursts bursts is accessed
		int max_dram_bursts = 65536;
		int access_bits = (int)Math.ceil(Math.log(max_dram_bursts) / Math.log(2));
		
		if (!lmem_linear_access){
			DFEVar mem_to_mem = io.scalarInput("mem_to_mem", dfeBool());
			DFEVar dram_bursts = io.scalarInput("dram_bursts", dfeUInt(32));
			DFEVar burst_size = io.scalarInput("burst_size", dfeUInt(8));

			int mem_stream_bits = (int)Math.ceil(Math.log(max_streams) / Math.log(2));
			DFEVar mem_streams = io.scalarInput("mem_streams", dfeUInt(mem_stream_bits));

			Count.Params param = control.count.makeParams(2)
				.withMax(2)
				.withWrapMode(WrapMode.COUNT_LT_MAX_THEN_WRAP);
			Counter romAddrCycleCnt = control.count.makeCounter(param);
			DFEVar romAddrCycle = romAddrCycleCnt.getCount() === 0;

			param = control.count.makeParams(32)
				.withEnable(romAddrCycle)
				.withMax(dram_bursts)
				.withWrapMode(WrapMode.STOP_AT_MAX);
			Counter romAddrMaxCntr = control.count.makeCounter(param);
			DFEVar romAddrMax = romAddrMaxCntr.getCount()*burst_size.cast(dfeUInt(32)) >= dram_bursts;

			param = control.count.makeParams(32)
				.withEnable(romAddrCycle);
			Counter romAddrCnt = control.count.makeCounter(param);
			DFEVar romAddr = romAddrCnt.getCount();

			for (int i = 0; i < max_streams; i++){
				// Sequence of address to access, to test if order of addr matters
				// Separate the read and write addr, to test if same/different addr
				// would cause any change in performance

				Memory<DFEVar> mappedRomR = mem.alloc(dfeUInt(32), max_dram_bursts);
				mappedRomR.mapToCPU("dram_addrR"+i);
				DFEVar dramAddrR = mappedRomR.read(romAddr.slice(0, access_bits).cast(dfeUInt(access_bits)));

				Memory<DFEVar> mappedRomW = mem.alloc(dfeUInt(32), max_dram_bursts);
				mappedRomW.mapToCPU("dram_addrW"+i);
				DFEVar dramAddrW = mappedRomW.read(romAddr.slice(0, access_bits).cast(dfeUInt(access_bits)));

				LMemCommandStream.makeKernelOutput(
						"from_dram_cmd"+i,
						~romAddrMax & mem_streams >= i & romAddrCycle,	// control
						dramAddrR,	// address
						burst_size,	// size
						constant.var(dfeUInt(8), 1),	// inc
						constant.var(dfeUInt(4), 0),	// stream
						constant.var(dfeBool(), 0));	// tag 

				LMemCommandStream.makeKernelOutput(
						"to_dram_cmd"+i,
						mem_to_mem & ~romAddrMax & mem_streams >= i & romAddrCycle,	// control
						dramAddrW,	// address
						burst_size,	// size
						constant.var(dfeUInt(8), 1),	// inc
						constant.var(dfeUInt(4), 0),	// stream
						constant.var(dfeBool(), 0));	// tag 

				debug.printf("[Cmd] addrR:%d  addrW:%d  stop:%d  romAddrCycle:%d\n", dramAddrR, dramAddrW, romAddrMax, romAddrCycleCnt.getCount());
			}
		}
	}
}
