# Makefile ---- Thomas Chau 2013
# Tested for MaxCompiler 2013.2.2

# Project settings
PRJ=LMemExample
PKG=lmemexample
DFEModel=MAX3424A
enableMPCX=false

# Paths
MAXDIR=./$(PRJ)_$(DFEModel)_DFE/results
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

all: build

# ---- DFE ----

$(MAXDIR)/$(PRJ).max: $(ENGINEFILES)
	$(MAXJC) $(JFLAGS) $(ENGINEFILES)
	MAXAPPJCP=. MAXSOURCEDIRS='../../EngineCode/src' MAXAPPPKG=$(PKG) $(MAXJAVARUN) -m 8192 $(PRJ)Manager DFEModel=$(DFEModel) maxFileName=$(PRJ) target='DFE' enableMPCX=$(enableMPCX) 
	cp $(MAXDIR)/$(PRJ).h ./Maxfiles.h

$(PRJ)_dfe.o: $(MAXDIR)/$(PRJ).max
	$(SLICCOMPILE) $< $@  

$(PRJ)_dfec.o: $(CPUFILES)
	$(CC) $< $(CFLAGS) -DDESIGN_NAME=$(PRJ) -c -o $@

$(PRJ)_dfe: $(PRJ)_dfe.o $(PRJ)_dfec.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) 

build: $(PRJ)_dfe

run:
	LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./${PRJ}_dfe

# ---- Clean ----

clean:
	rm -f $(PRJ)_dfe

distclean: clean
	rm -rf $(PKG) $(PRJ)_$(DFEModel)_DFE Maxfiles.h *.o *~

# ---- Debug ----

debug:

