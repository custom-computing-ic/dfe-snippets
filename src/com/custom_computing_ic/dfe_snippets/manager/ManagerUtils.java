
package com.custom_computing_ic.dfe_snippets.manager;

import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.DebugLevel;
import com.maxeler.maxcompiler.v2.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v2.build.EngineParameters;
import com.maxeler.maxcompiler.v2.managers.engine_interfaces.EngineInterface;
import com.maxeler.maxcompiler.v2.managers.engine_interfaces.EngineInterface.*;
import com.maxeler.maxcompiler.v2.managers.engine_interfaces.InterfaceParam;
import com.maxeler.maxcompiler.v2.managers.engine_interfaces.CPUTypes;
import com.maxeler.maxcompiler.v2.managers.DFEModel;
import com.maxeler.maxcompiler.v2.managers.BuildConfig;

import java.util.HashMap;

public class ManagerUtils {

  public static void setDRAMFreq(CustomManager manager, EngineParameters ep, int freq) {
    MemoryControllerConfig memCfg = new MemoryControllerConfig();

    HashMap<Integer, CustomManager.LMemFrequency> intToFreq =
                new HashMap<Integer, CustomManager.LMemFrequency>();

    if (ep.getDFEModel()==DFEModel.MAIA){
        memCfg.setEnableParityMode(true, true, 72, false);
        memCfg.setMAX4qMode(true);
        memCfg.setDataReadFIFOExtraPipelineRegInFabric(true);  // for easier meeting LMem timing

        intToFreq.put(400, CustomManager.LMemFrequency.MAX4MAIA_400);
        intToFreq.put(533, CustomManager.LMemFrequency.MAX4MAIA_533);
        intToFreq.put(666, CustomManager.LMemFrequency.MAX4MAIA_666);
        intToFreq.put(733, CustomManager.LMemFrequency.MAX4MAIA_733);
        intToFreq.put(800, CustomManager.LMemFrequency.MAX4MAIA_800);
    }
    else
    {
        intToFreq.put(300, CustomManager.LMemFrequency.MAX3_300);
        intToFreq.put(333, CustomManager.LMemFrequency.MAX3_333);
        intToFreq.put(350, CustomManager.LMemFrequency.MAX3_350);
        intToFreq.put(400, CustomManager.LMemFrequency.MAX3_400);
    }
    manager.config.setOnCardMemoryFrequency(intToFreq.get(freq));
    manager.config.setMemoryControllerConfig(memCfg);
  }

  // A generic DRAM write interface
  public static EngineInterface interfaceWrite(String name, String fromCpuStream, String cpu2lmemStream) {
    EngineInterface ei = new EngineInterface(name);
    CPUTypes TYPE = CPUTypes.INT;
    InterfaceParam size = ei.addParam("size_bytes", TYPE);
    InterfaceParam start = ei.addParam("start_bytes", TYPE);
    ei.setStream(fromCpuStream, CPUTypes.UINT8, size);
    ei.setLMemLinear(cpu2lmemStream, start, size);
    ei.ignoreAll(Direction.IN_OUT);
    return ei;
  }

  // A generic DRAM read interface
  public static EngineInterface interfaceRead(String name, String toCpuStream, String lmem2cpuStream) {
    EngineInterface ei = new EngineInterface(name);
    CPUTypes TYPE = CPUTypes.INT;
    InterfaceParam size = ei.addParam("size_bytes", TYPE);
    InterfaceParam start = ei.addParam("start_bytes", TYPE);
    ei.setStream(toCpuStream, CPUTypes.UINT8, size);
    ei.setLMemLinear(lmem2cpuStream, start, size);
    ei.ignoreAll(Direction.IN_OUT);
    return ei;
  }

  // Enable debugging with Stream Status for the given manager
  public static void debug(CustomManager manager) {
    DebugLevel dbgLevel = new DebugLevel();
    dbgLevel.setHasStreamStatus(true);
    manager.debug.setDebugLevel(dbgLevel);
  }

  // Setting up basic build parameters for a given manager
  public static void setFullBuild(CustomManager m, BuildConfig.Effort eff, int numCostTables, int numThreads)
  {
    BuildConfig c = new BuildConfig(BuildConfig.Level.FULL_BUILD);
    c.setBuildEffort(eff);
    c.setMPPRCostTableSearchRange(1, numCostTables);
    c.setMPPRParallelism(numThreads);
    m.setBuildConfig(c);
  }

  // Setting up basic build parameters for a given manager
  public static void setFullBuild(CustomManager m, int numCostTables, int numThreads)
  {
    setFullBuild(m, BuildConfig.Effort.HIGH, numCostTables, numThreads);
  }
}

