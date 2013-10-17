# Makefile ---- Thomas Chau 2013
# Tested for MaxCompiler 2013.2.2

# Project settings
PRJ=LMemExample
PKG=lmemexample
DFEModel=MAX3424A
enableMPCX=false

# Paths
MAXDIR=./$(PRJ)_$(DFEModel)_DFE_SIM/results
MAXELEROSDIR_SIM:=$(MAXCOMPILERDIR)/lib/maxeleros-sim

# Source files
ENGINEFILES=$(wildcard ../../EngineCode/src/$(PKG)/*.maxj)
CPUFILES=$(wildcard ../../CPUCode/*.c)

# Command alias
CC=icc
MAXFILECOMPILE=maxfilecompile
SLICCOMPILE=sliccompile
MAXGUESSBUILDDIR=maxGuessBuildDir
MAXFILESTITCH=maxfilestitch
MAXJAVARUN=maxJavaRun
MAXJC=maxjc
MAXDEBUG=maxdebug
MAXRENDERGRAPHS=maxRenderGraphs
MAXCOMPILERSIM=maxcompilersim

# Compiler settings
JFLAGS=-cp $(MAXCOMPILERDIR)/lib/MaxCompiler.jar -1.6 -d .
CFLAGS=-std=c99 -openmp -Wall -I. -I${MAXCOMPILERDIR}/include -I${MAXCOMPILERDIR}/include/slic -I${MAXELEROSDIR}/include -D_XOPEN_SOURCE=600
LFLAGS=-L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib -lmaxeleros -lslic -lm -lpthread -openmp

all: runsim

# ---- Simulation ----

$(MAXDIR)/$(PRJ).max: $(ENGINEFILES)
	$(MAXJC) $(JFLAGS) $(ENGINEFILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../../EngineCode/src' MAXAPPPKG=$(PKG) $(MAXJAVARUN) -m 8192 $(PRJ)Manager DFEModel=$(DFEModel) maxFileName=$(PRJ) target='DFE_SIM' enableMPCX=$(enableMPCX) 
	cp $(MAXDIR)/$(PRJ).h ./Maxfiles.h

$(PRJ)_sim.o: $(MAXDIR)/$(PRJ).max
	$(SLICCOMPILE) $< $@  

$(PRJ)_simc.o: $(CPUFILES)
	$(CC) $< $(CFLAGS) -D__SIM__ -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_sim: $(PRJ)_sim.o $(PRJ)_simc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) 

runsim: $(PRJ)_sim
	$(MAXCOMPILERSIM) -n $(USER)a -c3424 restart
	SLIC_CONF="use_simulation=$(USER)a" LD_PRELOAD=$(MAXELEROSDIR_SIM)/lib/libmaxeleros.so ./$(PRJ)_sim $(USER)a0:$(USER)a
	$(MAXCOMPILERSIM) -n $(USER)a -c3424 stop

stopsim:
	$(MAXCOMPILERSIM) -n $(USER)a -c3424 stop

# ---- Clean ----

clean:
	rm -f $(PRJ)_sim

distclean: clean
	rm -rf $(PKG) $(PRJ)_$(DFEModel)_DFE_SIM Maxfiles.h *.o *~

# ---- Debug ----

debug:

