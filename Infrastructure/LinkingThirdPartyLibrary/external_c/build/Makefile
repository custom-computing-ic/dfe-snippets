HARDWARELIBROOT=../..
HARDWARE_SO_DIR=$(HARDWARELIBROOT)/build
HARDWARE_INCLUDE_DIR=$(HARDWARELIBROOT)/include
HARDWAREPRJ=LinkingThirdPartyLibrary

SOFTWAREROOT=..
SOFTWAREPRJ=TargetCPUCode

# ---- Paths ----
ifndef $(MAXCOMPILER_BUILD_DIR)
	MAXCOMPILER_BUILD_DIR=.
endif
DFEMAXDIR=$(MAXCOMPILER_BUILD_DIR)/$(PRJ)_$(DFEModel)_DFE/results

COMPILER_CFLAGS = -fopenmp  -std=c99
OPT_FLAGS = -march=native -O3

CFLAGS= #$(COMPILER_CFLAGS) $(OPT_FLAGS) -Wall -I${MAXCOMPILERDIR}/include -I${MAXCOMPILERDIR}/include/slic -I${MAXELEROSDIR}/include -D_XOPEN_SOURCE=600
LFLAGS= -L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib -lmaxeleros # -lslic -lm -lpthread
EXTRA_CFLAGS?= -fPIC
ALL_FLAGS = $(CFLAGS) $(EXTRA_CFLAGS)

all: sim dfe
	echo $(SOFTWAREPRJ)
.PHONY: sim dfe clean

sim: $(SOFTWAREPRJ)_sim
dfe: $(SOFTWAREPRJ)_dfe
clean:
	rm -f *.o *.so *_sim $(SOFTWAREPRJ)_sim $(SOFTWAREPRJ)_dfe

# ---- Simulation ----

$(SOFTWAREPRJ)_simc.o: ../$(SOFTWAREPRJ).c
	$(CC)  $< $(ALL_FLAGS) -I$(HARDWARE_INCLUDE_DIR)/ -c -o $@

$(SOFTWAREPRJ)_sim: $(SOFTWAREPRJ)_simc.o
	$(CC) $(ALL_FLAGS) -L$(HARDWARE_SO_DIR) -l$(HARDWAREPRJ)_sim  -o $@ $^ $(LFLAGS)


# ---- DFE ----

$(SOFTWAREPRJ)_dfec.o: ../$(SOFTWAREPRJ).c
	$(CC) $< $(ALL_FLAGS) -I$(DFEMAXDIR) -DDESIGN_NAME=$(PRJ) -c -o $@

$(SOFTWAREPRJ)_dfe: $(SOFTWAREPRJ)_dfec.o
	$(CC) $(ALL_FLAGS) -o $@ $^ $(LFLAGS) -L$(HARDWARE_SO_DIR) -l$(HARDWAREPRJ)_dfe

