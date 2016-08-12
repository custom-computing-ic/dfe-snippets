#include <string.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <iomanip>
#include <omp.h>
#include "MultiChannelMemBandwidth.h"
#include "MaxSLiCInterface.h"

/* set to same values as in Manager */
#define BPC 3 // bursts per memory command
#define N_CHANNEL 4 // number of memory channels (must be <= 6)

#define MEM_FOOTPRINT 1000 // memory footprint size in bytes (must be <= 8GB)
#define TICKS 400 // number of input items to read
#define BURST_SIZE 64
#define N_RUNS 5

// input data type
struct in_t {
  uint32_t val[BPC*BURST_SIZE/4];
};

// write data to lmem
void writeData(in_t *data, uint64_t n_bytes, max_engine_t *engine);

// run benchmark
double runDfe(uint32_t *addr, uint32_t *out, max_engine_t *engine);


int main(void)
{
  int n_threads = omp_get_max_threads();

  uint32_t *addr = new uint32_t [TICKS]; // command kernel input
  uint32_t out[4*N_CHANNEL]; // passthrough kernel output

  // generate input data
  std::cout << "Input = " << MEM_FOOTPRINT/1e+9 << " GB" << std::endl;

  uint32_t in_size = MEM_FOOTPRINT/sizeof(in_t);

  in_t *in = new in_t [in_size];

#pragma omp parallel for num_threads(n_threads)
  for(uint32_t i = 0; i < in_size; i++) {
    in[i].val[0] = i;
  }

  // generate addresses
#pragma omp parallel for num_threads(n_threads)
  for (uint32_t i = 0; i < TICKS; i++) {
    addr[i] = rand()%in_size;
  }

  // load maxfile
  max_file_t *maxfile = MultiChannelMemBandwidth_init();
  max_engine_t *engine = max_load(maxfile, "*");

  // transfer input to DFE LMem
  std::cout << "Writing data to LMem." << std::endl;
  writeData(in, in_size*sizeof(in_t), engine);

  // run kernel
  std::cout << "Running DFE." << std::endl;
  double time = 0.0;
  for (int i = 0; i < N_RUNS; i++) {
    time += runDfe(addr, out, engine);
  }
  time /= N_RUNS;
  std::cout << "time taken = " << time << std::endl;
  std::cout << "bandwidth = " << std::setprecision(2) <<
    ((TICKS*sizeof(in_t)*N_CHANNEL)/time)/1e+9 << "GB/s" << std::endl;

  // test output correctness
  uint32_t val = in[addr[TICKS-1]].val[0];
  for (int i = 0; i < N_CHANNEL; i++) {
    if (val != out[i*4]) {
      printf("test failed: output from DFE did not match CPU: %u %u\n",
          val, out[i*4]);
      exit(1);
    }
  }
  std::cout << "test passed!" << std::endl;

  // cleanup
  max_unload(engine);
  delete[] in;
  delete[] addr;

  return 0;
}

void writeData(in_t *data, uint64_t n_bytes, max_engine_t *engine)
{
  MultiChannelMemBandwidth_Write_actions_t write_action;
  write_action.param_nBytes = n_bytes;
  write_action.instream_hostToMger = (uint64_t*)data;

  MultiChannelMemBandwidth_Write_run(engine, &write_action);
}

double runDfe(uint32_t *addr, uint32_t *out, max_engine_t *engine)
{
  struct timeval  tv1, tv2;
  MultiChannelMemBandwidth_Exec_actions_t exec_action;
  exec_action.param_ticks = TICKS;

#if N_CHANNEL > 0
  exec_action.instream_addr0 = addr;
  exec_action.outstream_k_out0 = &out[0];
#endif
#if N_CHANNEL > 1
  exec_action.instream_addr1 = addr;
  exec_action.outstream_k_out1 = &out[4];
#endif
#if N_CHANNEL > 2
  exec_action.instream_addr2 = addr;
  exec_action.outstream_k_out2 = &out[8];
#endif
#if N_CHANNEL > 3
  exec_action.instream_addr3 = addr;
  exec_action.outstream_k_out3 = &out[12];
#endif
#if N_CHANNEL > 4
  exec_action.instream_addr4 = addr;
  exec_action.outstream_k_out4 = &out[16];
#endif
#if N_CHANNEL > 5
  exec_action.instream_addr5 = addr;
  exec_action.outstream_k_out5 = &out[20];
#endif

  gettimeofday(&tv1, NULL);
  MultiChannelMemBandwidth_Exec_run(engine, &exec_action);
  gettimeofday(&tv2, NULL);

  return ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
      (double) (tv2.tv_sec - tv1.tv_sec));

}
