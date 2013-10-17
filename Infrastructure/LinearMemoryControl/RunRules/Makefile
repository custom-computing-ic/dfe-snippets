###################################
# Makefile ---- Thomas Chau 2013  #
###################################
# Tested for MaxCompiler 2013.2.2
# For SLiC interface only
# If you are using MaxCompilerRT, 
# use maxfilecompile instead of sliccompile
# and change LFLAGS -lslic to -lmaxcompilerrt

# ---- Project settings ----
# maxnode* and maxstation* - DFEModel=MAX3424A
# maia* - DFEModel=MAIA
PRJ=LMemExample
PKG=lmemexample
DFEModel=MAX3424A
ifeq ($(DFEModel),MAX3424A)
MPCX=false
DEVICENUM=3424
SLICCONFIG=
else
MPCX=true
DEVICENUM=MAIA
SLIC_CONF="default_engine_resource = 192.168.0.10"
endif

# ---- Paths ----
SIMMAXDIR=$(PRJ)_$(DFEModel)_DFE_SIM/results
DFEMAXDIR=$(PRJ)_$(DFEModel)_DFE/results
MAXELEROSDIR_SIM:=$(MAXCOMPILERDIR)/lib/maxeleros-sim

# ---- Source files ----
ENGINEFILES=$(wildcard ../EngineCode/src/$(PKG)/*.maxj)
CPUFILES=$(wildcard ../CPUCode/*.c)

# ---- Command alias ----
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

# ---- Compiler settings ----
JFLAGS=-cp $(MAXCOMPILERDIR)/lib/MaxCompiler.jar -1.6 -d .
CFLAGS=-std=c99 -openmp -Wall -I. -I${MAXCOMPILERDIR}/include -I${MAXCOMPILERDIR}/include/slic -I${MAXELEROSDIR}/include -D_XOPEN_SOURCE=600
LFLAGS=-L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib -lmaxeleros -lslic -lm -lpthread -openmp

all: runsim build

# ---- Simulation ----

$(SIMMAXDIR)/$(PRJ).max: $(ENGINEFILES)
	$(MAXJC) $(JFLAGS) $(ENGINEFILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../EngineCode/src' MAXAPPPKG=$(PKG) $(MAXJAVARUN) -v -m 8192 $(PRJ)Manager DFEModel=$(DFEModel) maxFileName=$(PRJ) target='DFE_SIM' enableMPCX=$(MPCX)
	cp $(SIMMAXDIR)/$(PRJ).h Maxfiles.h

$(PRJ)_sim.o: $(SIMMAXDIR)/$(PRJ).max
	$(SLICCOMPILE) $< $@  

$(PRJ)_simc.o: $(CPUFILES)
	$(CC) $< $(CFLAGS) -D__SIM__ -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_sim: $(PRJ)_sim.o $(PRJ)_simc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) 

runsim: $(PRJ)_sim
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) restart
	SLIC_CONF+="use_simulation=$(USER)a" LD_PRELOAD=$(MAXELEROSDIR_SIM)/lib/libmaxeleros.so ./$(PRJ)_sim $(USER)a0:$(USER)a
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) stop

stopsim:
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) stop

# ---- DFE ----

$(DFEMAXDIR)/$(PRJ).max: $(ENGINEFILES)
	$(MAXJC) $(JFLAGS) $(ENGINEFILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../EngineCode/src' MAXAPPPKG=$(PKG) $(MAXJAVARUN) -v -m 8192 $(PRJ)Manager DFEModel=$(DFEModel) maxFileName=$(PRJ) target='DFE' enableMPCX=$(MPCX) 
	cp $(DFEMAXDIR)/$(PRJ).h Maxfiles.h

$(PRJ)_dfe.o: $(DFEMAXDIR)/$(PRJ).max
	$(SLICCOMPILE) $< $@  

$(PRJ)_dfec.o: $(CPUFILES)
	$(CC) $< $(CFLAGS) -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_dfe: $(PRJ)_dfe.o $(PRJ)_dfec.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) 

build: $(PRJ)_dfe

run:
	LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./${PRJ}_dfe

# ---- Clean ----

cleansim:
	rm -f $(PRJ)_sim *~

cleandfe:
	rm -f $(PRJ)_dfe *~

clean: cleansim cleandfe

distcleansim: cleansim
	rm -rf $(PKG) Maxfiles.h *.o
	rm -rf $(PRJ)_$(DFEModel)_DFE_SIM

distcleandfe: cleandfe
	rm -rf $(PKG) Maxfiles.h *.o
	rm -rf $(PRJ)_$(DFEModel)_DFE

distclean: distcleansim distcleandfe

# ---- Debug ----

debug:
	maxdebug -v -g $(PRJ)Graph -s $(PRJ)Graph -c -k $(PRJ)Kernel -d $(USER)a0:$(USER)a $(SIMMAXDIR)/$(PRJ).max

