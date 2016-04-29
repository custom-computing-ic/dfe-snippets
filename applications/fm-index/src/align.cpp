/* align.cpp ----- James Arram 2014 */

/*
 * align source code file
 */

#include "align.hpp"

// kernel input
struct in_t {
  uint32_t id;
  uint8_t pck_sym[42];
  uint8_t len;
  uint8_t is_pad;
};

// kernel output
struct out_t {
  uint32_t id;
  uint32_t low;
  uint32_t high;
  uint32_t pad;
};

using namespace std;

// exact match kernel latency
uint32_t latency = 200;

// get partition sizes
void getPartSize(uint32_t *part_size, uint32_t items);

// kernel write action
void actionWrite(index_t *index, uint64_t index_bytes,  max_engine_t **engine);

// kernel align action
void actionAlign(in_t **in, out_t **out, uint32_t *part_size, uint32_t high_init, 
		 max_engine_t **engine);

//align reads
void align(vector<read_t> &reads, index_t *index, uint64_t index_bytes,
	   uint32_t high_init)
{
  in_t *in[N_KRNL*N_DFE];
  out_t *out[N_KRNL*N_DFE];
  uint32_t part_size[N_KRNL*N_DFE];
  max_file_t *maxfile = NULL;
  max_engine_t *engine[N_DFE];
  max_group_t *group = NULL;
  struct timeval  tv1, tv2;
  
  // load exact match kernel bitstream                                            
  printf("\tloading exact match kernel bitstream ... "); fflush(stdout);
  gettimeofday(&tv1, NULL);
  maxfile = Em_init();
  group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "*", N_DFE);
  for (uint8_t i = 0; i < N_DFE; i++)
    engine[i] = max_lock_any(group);
  gettimeofday(&tv2, NULL);
  printf("OK [%.2f s]\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	 (double) (tv2.tv_sec - tv1.tv_sec));

  // write index to DRAM memory                                                   
  printf("\ttransferring index to DRAM ... "); fflush(stdout);
  gettimeofday(&tv1, NULL);
  actionWrite(index, index_bytes, engine);
  gettimeofday(&tv2, NULL);
  printf("OK [%.2f s]\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	 (double) (tv2.tv_sec - tv1.tv_sec));

  // generate kernel input
  getPartSize(part_size, reads.size());
  uint32_t offset = 0;
  for (uint8_t i = 0; i < N_KRNL*N_DFE; i++) {
    in[i] = new in_t [part_size[i]+latency];
    if (!in[i]) {
      printf("error: unable to allocate memory!\n");
      exit(1);
    }    
    memset(in[i], 0, part_size[i]+latency*sizeof(in_t));      
#pragma omp parallel for num_threads(N_THREADS)
    for (uint32_t j = 0; j < part_size[i]; j++) {
      uint32_t id = j+offset;
      in[i][j].id = id;
      in[i][j].len = reads[id].len;
      memcpy(in[i][j].pck_sym, reads[id].pck_sym, CEIL(reads[id].len, 4));
      in[i][j].is_pad = 0;
    }
    for (uint8_t j = 0; j < latency; j++) {
      if (j < latency - 1)
	in[i][part_size[i]+j].is_pad = 1;
      else 
	in[i][part_size[i]+j].is_pad = 2;
    }
    offset += part_size[i];
  }

  // exact align reads                                                            
  printf("\texact aligning reads ... "); fflush(stdout);
  gettimeofday(&tv1, NULL);
  for (uint8_t i = 0; i < N_KRNL*N_DFE; i++){
    out[i] = new out_t [part_size[i]];
    if (!out[i]) {
      printf("error: unable to allocate memory!\n");
      exit(1);
    }    
    memset(out[i], 0, part_size[i]*sizeof(out_t));
  }
  actionAlign(in, out, part_size, high_init, engine);
  gettimeofday(&tv2, NULL);
  printf("OK [%.2f s]\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	 (double) (tv2.tv_sec - tv1.tv_sec));
  
  // parse output
  printf("\tparsing results ... "); fflush(stdout);
  gettimeofday(&tv1, NULL);
  for (uint8_t i = 0; i < N_KRNL*N_DFE; i++) {
#pragma omp parallel for num_threads(N_THREADS)
    for (uint32_t j = 0; j < part_size[i]; j++) {
      uint32_t id = out[i][j].id;
      if (out[i][j].low <= out[i][j].high) {
	reads[id].low = out[i][j].low;
	reads[id].high = out[i][j].high;
	reads[id].is_align = true;
      }
    }
  }
  gettimeofday(&tv2, NULL);
  printf("OK [%.2f s]\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	 (double) (tv2.tv_sec - tv1.tv_sec));
    
  // cleanup 
  for (int i = 0; i < N_DFE; i++)
    max_unlock(engine[i]);
  max_unload_group(group);
  max_file_free(maxfile);
  for (int i = 0; i < N_KRNL*N_DFE; i++) {
    delete[] in[i];
    delete[] out[i];
  }
}

// get partition size                                                 
void getPartSize(uint32_t *part_size, uint32_t items)
{
  for (uint8_t i = 0; i < N_KRNL*N_DFE; i++) {
    if (items%(N_KRNL*N_DFE)!=0) {
      part_size[i] = CEIL(items, N_KRNL*N_DFE);
      items-=1;
    }
    else
      part_size[i] = items/(N_KRNL*N_DFE);
  }
}

// kernel write action
void actionWrite(index_t *index, uint64_t index_bytes,  max_engine_t **engine)
{
  Em_Write_actions_t *em_write[N_DFE];
  for (size_t i = 0; i < T_SIZE; i++) {
    for (int j = 0; j < N_DFE; j++) {
      em_write[j] = new Em_Write_actions_t;
      em_write[j]->param_nBytes = index_bytes/T_SIZE;
      em_write[j]->param_offset = i*(index_bytes/T_SIZE);
      em_write[j]->instream_indexToMger =
        (uint64_t*)&index[i*(index_bytes/T_SIZE/BURST_BYTES)];
    }
#pragma omp parallel for num_threads(N_DFE)
    for (int j = 0; j < N_DFE; j++) {
      Em_Write_run(engine[j], em_write[j]);
      delete em_write[j];
    }
  }
}

// kernel align action
void actionAlign(in_t **in, out_t **out, uint32_t *part_size, uint32_t high_init, 
		 max_engine_t **engine)
{
  uint64_t n_bytes_in[N_KRNL*N_DFE];
  uint64_t n_bytes_out[N_KRNL*N_DFE];
  for (uint8_t i = 0; i < N_KRNL*N_DFE; i++) {
    n_bytes_in[i] = (part_size[i]+latency)*sizeof(in_t);
    n_bytes_out[i] = part_size[i]*sizeof(out_t);
  }
  
  Em_Align_actions_t *em_align[N_DFE];
  for (uint8_t i = 0; i < N_DFE; i++) {
    em_align[i] = new Em_Align_actions_t;
    em_align[i]->param_offset = latency;
    em_align[i]->param_highInit = high_init;
    em_align[i]->param_nBytesInput = &n_bytes_in[i*N_KRNL];
    em_align[i]->param_nBytesOutput = &n_bytes_out[i*N_KRNL];
  
#if N_KRNL > 0
    em_align[i]->instream_readIn0 = (uint64_t*)in[i*N_KRNL];
    em_align[i]->outstream_alignOut0 = (uint64_t*)out[i*N_KRNL];
#endif

#if N_KRNL > 1
    em_align[i]->instream_readIn1 = (uint64_t*)in[(i*N_KRNL)+1];
    em_align[i]->outstream_alignOut1 = (uint64_t*)out[(i*N_KRNL)+1];
#endif 

#if N_KRNL > 2
    em_align[i]->instream_readIn2 = (uint64_t*)in[(i*N_KRNL)+2];
    em_align[i]->outstream_alignOut2 = (uint64_t*)out[(i*N_KRNL)+2];
#endif 

#if N_KRNL > 3
    em_align[i]->instream_readIn3 = (uint64_t*)in[(i*N_KRNL)+3];
    em_align[i]->outstream_alignOut3 = (uint64_t*)out[(i*N_KRNL)+3];
#endif

#if N_KRNL > 4
    em_align[i]->instream_readIn4 = (uint64_t*)in[(i*N_KRNL)+4];
    em_align[i]->outstream_alignOut4 = (uint64_t*)out[(i*N_KRNL)+4];
#endif 

#if N_KRNL > 5
    em_align[i]->instream_readIn5 = (uint64_t*)in[(i*N_KRNL)+5];
    em_align[i]->outstream_alignOut5 = (uint64_t*)out[(i*N_KRNL)+5];
#endif    
  }
#pragma omp parallel for num_threads(N_DFE)
  for (uint32_t i = 0; i < N_DFE; i++) {
    Em_Align_run(engine[i], em_align[i]);
    delete em_align[i];
  }
}