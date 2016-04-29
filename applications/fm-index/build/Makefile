# Makefile ----- James Arram 2014

# DFE parameters
DFEModel=MAIA
MPCX=true
DEVICENUM=MAIA
SLIC_CONF="default_engine_resource=192.168.0.12;default_wfi_timeout=60;default_pcie_timeout=60"

# project name
PRJ=align

# design names
D1=Em

# paths
D1SIMDIR=$(D1)_$(DFEModel)_DFE_SIM/results
D1DFEDIR=$(D1)_$(DFEModel)_DFE/results
MAXELEROSDIR_SIM:=$(MAXCOMPILERDIR)/lib/maxeleros-sim
SRCDIR=../src
OBJDIR=obj

# source files
D1FILES=$(wildcard $(SRCDIR)/*.maxj)
CPUFILES=$(wildcard $(SRCDIR)/*.cpp)
DEPFILES=$(wildcard $(SRCDIR)/*.hpp)
OBJFILES=$(patsubst $(SRCDIR)/%.cpp, obj/%.o, $(CPUFILES))

# compiler 
CC=g++
MAXFILECOMPILE=maxfilecompile
SLICCOMPILE=sliccompile
MAXGUESSBUILDDIR=maxGuessBuildDir
MAXFILESTITCH=maxfilestitch
MAXJAVARUN=maxJavaRun
MAXJC=maxjc
MAXDEBUG=maxdebug
MAXRENDERGRAPHS=maxRenderGraphs
MAXCOMPILERSIM=maxcompilersim

JFLAGS=-cp $(MAXCOMPILERDIR)/lib/MaxCompiler.jar -1.6 -d .
CFLAGS=-O3 -g -fopenmp -Wall -Wextra -I${MAXCOMPILERDIR}/include -I${MAXCOMPILERDIR}/include/slic -I${MAXELER\OSDIR}/include_
LFLAGS=-L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib -lmaxeleros -lslic -lm -lpthread -fopenmp

# simulation
$(D1SIMDIR)/$(D1).max: $(D1FILES)
	$(MAXJC) $(JFLAGS) $(D1FILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../src' $(MAXJAVARUN) -v -m 8192 $(D1)Manager DFEModel=$(DFEModel) maxFileName=$(D1) target='DFE_SIM' enableMPCX=$(MPCX)
	cp $(D1SIMDIR)/$(D1).h $(D1SIMDIR)/Maxfiles.h
	@ mkdir -p class
	mv *.class class/

$(OBJDIR)/$(D1)_sim.o: $(D1SIMDIR)/$(D1).max
	@ mkdir -p obj
	$(SLICCOMPILE) $< $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPFILES)
	@ mkdir -p obj
	$(CC) $< $(CFLAGS) -I$(D1SIMDIR) -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_sim: $(OBJDIR)/$(D1)_sim.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

runsim: $(PRJ)_sim
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) -k restart
	SLIC_CONF+="use_simulation=$(USER)a" LD_PRELOAD=$(MAXELEROSDIR_SIM)/lib/libmaxeleros.so \
	./$(PRJ)_sim chr1.fmt.trim chr1.trim.fastq
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) stop

stopsim:
	$(MAXCOMPILERSIM) -n $(USER)a -c$(DEVICENUM) stop

# hardware
$(D1DFEDIR)/$(D1).max: $(D1FILES)
	$(MAXJC) $(JFLAGS) $(D1FILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../src' $(MAXJAVARUN) -v -m 8192 $(D1)Manager DFEModel=$(DFEModel) maxFileName=$(D1) target='DFE' enableMPCX=$(MPCX)
	cp $(D1DFEDIR)/$(D1).h $(D1DFEDIR)/Maxfiles.h
	rm $(D1DFEDIR)/$(D1).h
	rm $(D1DFEDIR)/$(D1).max
	cp $(D1DFEDIR)/../scratch/$(D1).h $(D1DFEDIR)/$(D1).h
	cp $(D1DFEDIR)/../scratch/altera_quartus/$(D1).max $(D1DFEDIR)/$(D1).max
	rm -rf $(D1DFEDIR)/../scratch
	@ mkdir -p class
	mv *.class class/

$(OBJDIR)/$(D1)_dfe.o: $(D1DFEDIR)/$(D1).max
	@ mkdir -p obj
	$(SLICCOMPILE) $< $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPFILES)
	@ mkdir -p obj
	$(CC) $< $(CFLAGS) -I$(D1DFEDIR) -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_dfe: $(OBJDIR)/$(D1)_dfe.o  $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

build: $(PRJ)_dfe

run:
	SLIC_CONF=$(SLIC_CONF) LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so \
	./${PRJ}_dfe /mnt/ssd2/jma11/index/basic/hg38.fmt /mnt/ssd2/jma11/reads/hg38.fastq