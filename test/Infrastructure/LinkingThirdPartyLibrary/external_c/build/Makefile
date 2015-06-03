# ---- Paths ----
HARDWARELIBROOT=../..
HARDWARE_SO_DIR=${HARDWARELIBROOT}/build
HARDWARE_INCLUDE_DIR=${HARDWARELIBROOT}/include
HARDWAREPRJ=LinkingThirdPartyLibrary

SOFTWAREROOT=..
SOFTWAREPRJ=TargetCPUCode

COMPILER_CFLAGS = -fopenmp  -std=c99
OPT_FLAGS = -march=native -O3
CFLAGS= ${COMPILER_CFLAGS} ${OPT_FLAGS} -Wall
LFLAGS= -L${MAXELEROSDIR}/lib -lmaxeleros -fPIC
ALL_FLAGS = ${CFLAGS} ${EXTRA_CFLAGS}

all: sim dfe
	echo ${SOFTWAREPRJ}
.PHONY: sim dfe clean

sim: ${SOFTWAREPRJ}_sim
dfe: ${SOFTWAREPRJ}_dfe
clean:
	rm -f *.o *.so *_sim ${SOFTWAREPRJ}_sim ${SOFTWAREPRJ}_dfe

# ---- Simulation ----

${SOFTWAREPRJ}_simc.o: ../${SOFTWAREPRJ}.c
	${CC}  $< ${ALL_FLAGS} -I${HARDWARE_INCLUDE_DIR}/ -c -o $@

${SOFTWAREPRJ}_sim: ${SOFTWAREPRJ}_simc.o
	${CC} ${ALL_FLAGS} -L${HARDWARE_SO_DIR} -l${HARDWAREPRJ}_sim  -o $@ $^ ${LFLAGS}


# ---- DFE ----

${SOFTWAREPRJ}_dfec.o: ../${SOFTWAREPRJ}.c
	${CC} $< ${ALL_FLAGS}  -DDESIGN_NAME=${PRJ} -c -o $@

${SOFTWAREPRJ}_dfe: ${SOFTWAREPRJ}_dfec.o
	${CC} ${ALL_FLAGS} -o $@ $^ ${LFLAGS} -L${HARDWARE_SO_DIR} -l${HARDWAREPRJ}_dfe

