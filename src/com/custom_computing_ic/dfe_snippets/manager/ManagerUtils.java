package com.custom_computing_ic.dfe_snippets.manager;

import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControlGroup;
import com.maxeler.maxcompiler.v2.managers.engine_interfaces.EngineInterface.*;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.DebugLevel;
import com.maxeler.maxcompiler.v2.managers.custom.CustomManager;
import com.maxeler.maxcompiler.v2.managers.custom.stdlib.MemoryControllerConfig;

public class ManagerUtils {

  // Probably only works for MAIA
  public static void setDRAM800(CustomManager manager) {
    MemoryControllerConfig memCfg = new MemoryControllerConfig();
    memCfg.setEnableParityMode(true, true, 72, false);
    memCfg.setMAX4qMode(true);
    manager.config.setOnCardMemoryFrequency(CustomManager.LMemFrequency.MAX4MAIA_800);
    manager.config.setMemoryControllerConfig(memCfg);
  }

  public static void debug(CustomManager manager) {
    DebugLevel dbgLevel = new DebugLevel();
    dbgLevel.setHasStreamStatus(true);
    manager.debug.setDebugLevel(dbgLevel);
  }

}

