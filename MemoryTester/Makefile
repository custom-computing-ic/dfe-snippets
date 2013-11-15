PRJ				=  mem_test
BUILD_TAG		=  _width1536_MAX4
BUILD_CARD		=  MAX4848A
#BUILD_CARD		=  MAX3424A

INCLUDES 		=	-I$(MAXELEROSDIR)/include -I$(MAXCOMPILERDIR)/include/slic
CC 				=	gcc
CXX 			=	g++
CFLAGS 			=	-g3 -fPIC -Wall $(INCLUDES)
LDFLAGS 		=	-Wl,--hash-style=both

SIM_ID			=	mem_test
USE_SLIC		=	1
include $(MAXCOMPILERDIR)/lib/Makefile.include
CFLAGS			+=	$(MAXCOMPILER_INC)
LDFLAGS			+=	$(MAXCOMPILER_LIBS)

MAXFILE_SIM_OBJ		=	maxfile_$(PRJ)_Sim$(BUILD_TAG).o
MAXFILE_DFE_OBJ		=	maxfile_$(PRJ)_DFE$(BUILD_TAG).o

ifeq ($(host), maia01.doc.ic.ac.uk)
	MAXCARD		=  MAX4848A
	MAIA_FLAG	=  1
	#SLIC_CONF_DFE = default_engine_resource=192.168.0.10;default_maxdebug_mode=2
	SLIC_CONF_DFE = default_engine_resource=192.168.0.10
else
	MAXCARD		=  MAX3424A
	MAIA_FLAG	=  0
	SLIC_CONF_DFE = default_wfi_timeout=30
endif

ifeq ($(BUILD_CARD), MAX4848A)
	enableMPCX	= true
else
	enableMPCX = false
endif

VALGRIND		= valgrind --leak-check=full  --track-origins=yes

MAX_BUILDPARAMS_SIM = DFEModel=$(BUILD_CARD)  target=DFE_SIM  maxFileName=$(PRJ)  enableMPCX=false  buildTag=$(BUILD_TAG)
MAX_BUILDPARAMS_DFE = DFEModel=$(BUILD_CARD)  target=DFE  maxFileName=$(PRJ)  enableMPCX=$(enableMPCX)  buildTag=$(BUILD_TAG)
SLIC_CONF_SIM	= verbose=false;max_socket=$(SIM_ID)

PRELOAD			= $(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/libmaxeleros.so:$(LD_PRELOAD)

SRCPATH			= src/

help:
	@echo "Targets:"
	@echo "    bitstream-sim  Compile a simulation bitstream"
	@echo "    bitstream-dfe  Compile a hardware bitstream"
	@echo "    compile-sim    Compile the CPU code for simulation"
	@echo "    compile-dfe    Compile the CPU code for hardware"
	@echo "    run-sim        Run the simulation process"
	@echo "    stop-sim       Stop the simulation process"
	@echo "    run-dfe        Run the hardware test"
	@echo "    stop-dfe       Stop the hardware test"
	@echo "    clean-sim      Delete all files for simulation"
	@echo "    clean          Delete all files (simulation and hardware)"

compile-sim: $(PRJ)_sim_host
compile-dfe: $(PRJ)_dfe_host

bitstream-sim:
	ant
	CLASSPATH=bin:$(CLASSPATH) maxJavaRun $(PRJ).SimManager $(MAX_BUILDPARAMS_SIM)

bitstream-dfe:
	ant
	CLASSPATH=bin:$(CLASSPATH) maxJavaRun $(PRJ).DFEManager $(MAX_BUILDPARAMS_DFE)

maxfile_%.o:
	echo  $(MAXGUESSBUILDDIR) -s $(patsubst maxfile_%.o,%,$@)
	$(SLICCOMPILE) \
		$(shell $(MAXGUESSBUILDDIR) -s $(patsubst maxfile_%.o,%,$@))/results/*.max $@ 

$(PRJ)_sim_host: $(MAXFILE_SIM_OBJ) $(SRCPATH)/$(PRJ).c
	$(CC) -I$(PRJ)_Sim$(BUILD_TAG)/results -rdynamic $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PRJ)_dfe_host: $(MAXFILE_DFE_OBJ) $(SRCPATH)/$(PRJ).c
	$(CC) -I$(PRJ)_DFE$(BUILD_TAG)/results -rdynamic $(CFLAGS) -o $@ $^ $(LDFLAGS)

run-sim: $(PRJ)_sim_host
	$(MAXCOMPILERDIR)/bin/maxcompilersim -c $(MAXCARD) -n $(SIM_ID) restart
	LD_PRELOAD="$(PRELOAD)" SLIC_CONF="$(SLIC_CONF_SIM)" LD_LIBRARY_PATH="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/:$(LD_LIBRARY_PATH)" \
			   ./$< $(PRJ)_Sim$(BUILD_TAG) $(MAIA_FLAG)
	$(MAXCOMPILERDIR)/bin/maxcompilersim -c $(MAXCARD) -n $(SIM_ID) stop

stop-sim:
	$(MAXCOMPILERDIR)/bin/maxcompilersim -c $(MAXCARD) -n $(SIM_ID) stop

run-dfe: $(PRJ)_dfe_host
	LD_LIBRARY_PATH="$(MAXELEROSDIR)/lib/" SLIC_CONF="$(SLIC_CONF_DFE)" ./$< $(PRJ)_DFE$(BUILD_TAG) $(MAIA_FLAG)

stop-dfe:
ifeq ($(MAIA_FLAG), 1)
	/opt/maxeler/maxeleros/utils/maxforceidle -a -r 192.168.0.10
else
	/opt/maxeler/maxeleros/utils/maxforceidle -a
endif

clean-sim:
	-$(MAXCOMPILERDIR)/bin/maxcompilersim -n $(SIM_ID) stop
	rm -f maxfile_$(PRJ)_Sim.o 
	rm -f $(PRJ)_sim_host
	rm -r $(PRJ)_Sim

clean:
	-$(MAXCOMPILERDIR)/bin/maxcompilersim -n $(SIM_ID) stop
	rm -f *.o 
	rm -f $(PRJ)_sim_host $(PRJ)_dfe_host
	rm -r bin $(PRJ)_Sim $(PRJ)_DFE$(BUILD_TAG)

sim: bitstream-sim compile-sim
dfe: bitstream-dfe compile-dfe
