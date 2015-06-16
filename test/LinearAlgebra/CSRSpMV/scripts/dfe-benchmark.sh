#!/bin/bash


params=(
    "Parameters.OMP_THREADS" 
    "Parameters.PIPES"
    "Parameters.STREAM_FREQ"
    "Results.CPU_TIME"
    "Results.DRAM_WRITE"
    "Results.DFE_ESTIMATED"
    "Results.DFE_ACTUAL"
    "Results.SPEEDUP"
)

nthreads=12

# Print header
echo -n "SIZE,"
for i in ${params[@]}
do
    echo -n $i, | sed 's/Parameters.//' | sed 's/Results.//'
done
echo

function extractParam() {
    echo "${2}" | grep "${1}" | sed "s/${1}[[:space:]]*=[[:space:]]*//" | sed "s/[[:space:]]*s//"
}

function addLine() {
    echo -n $size,
    for i in ${params[@]}
    do
        echo -n $( extractParam "$i" "${output}" ),
    done
    echo
}

# Use in hardware for actual results
function runDfe() {
    size=$1
    output="$( OMP_NUM_THREADS=$nthreads bash run-hw.sh build/bin/Eval_dfe a b -e ${nExpressions} -d ${nDays} )"
    addLine ${output}
}

# Use in simulation to test 
function runSim() {
    size=$1
    output="$( OMP_NUM_THREADS=$nthreads bash scripts/run-sim.sh build/bin/fpga_sim -s ${size})"
    addLine "${output}"
}

# range over some params (e.g. size) to generate a benchmark
for size in 384 3840 38400 384000
do
    runSim $size
done

# Sample output from running the benchmark
# Running benchmark
#    Parameters.OMP_THREADS      = 12
#    Parameters.DFE_PIPES        = 4
#    Parameters.FPGA_STREAM_FREQ = 200
# Running CPU benchmark
# Writing data to DRAM... 
#     (MB) = 384000
# Running DFE
#    Results.CPU_TIME       = 0.00444048 s
#    Results.DRAM_WRITE     = 1.02186 s
#    Results.FPGA_ESTIMATED = 0.000457764 s
#    Results.FPGA_ACTUAL    = 4.84342 s
#    Results.SPEEDUP        = 0.000916807
