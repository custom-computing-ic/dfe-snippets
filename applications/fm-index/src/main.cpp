/* main.cpp ----- James Arram 2015 */

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <stdint.h>
#include <vector>
#include "def.hpp"
#include "file.hpp"
#include "reads.hpp"
#include "index.hpp"
#include "align.hpp"

// allocate memory                
template<typename T> void allocate(T * &a, uint64_t n);

int main(int argc, char *argv[]) {
  
  FILE *fp = NULL;
  index_t *idx = NULL;
  char *ref = NULL;
  uint32_t *sai = NULL;
  std::vector<read_t> reads;
  uint64_t index_bytes;
  uint64_t f_size;
  uint32_t high_init;
  char f_name[128];

  // check usage
  if (argc != 3) {
    printf("usage: %s <fmt file> <fastq file>\n", argv[0]);
    exit(1);
  }

  // load index
  printf("loading index ... "); fflush(stdout);
  strcpy(f_name, argv[1]);
  strcat(f_name, ".idx");
  openFile(&fp, f_name, "rb");
  index_bytes = fileSizeBytes(fp);
  allocate(idx, index_bytes);
  readFile(fp, idx, index_bytes);
  fclose(fp);
  printf("OK\n");
  
  // load suffix array
  printf("loading suffix array ... "); fflush(stdout);
  strcpy(f_name, argv[1]);
  strcat(f_name, ".sai");
  openFile(&fp, f_name, "rb");
  f_size = fileSizeBytes(fp);
  allocate(sai, f_size);
  readFile(fp, sai, f_size);
  fclose(fp);
  printf("OK\n");
  
  // load reference genome
  printf("loading reference genome ... "); fflush(stdout);  
  openFile(&fp, argv[1], "rb");
  f_size = fileSizeBytes(fp);
  high_init = f_size;
  allocate(ref, f_size);
  readFile(fp, ref, f_size);
  fclose(fp);
  printf("OK\n");

  // load short reads
  printf("loading short reads ... "); fflush(stdout);
  openFile(&fp, argv[2], "r");
  loadReads(fp, reads);
  fclose(fp);
  printf("OK\n");

  // exact align reads
  for (int j = 0; j < N_RUNS; j++) {
    printf("aligning reads ... \n"); fflush(stdout);
    align(reads, idx, index_bytes, high_init);
    
    // run test bench 1 
    for (uint32_t i = 0; i < reads.size(); i++) {
      if (reads[i].is_align == false) {
	printf("\ntest bench 1 failed! %u\n", i);
	exit(1);
      }
      reads[i].is_align = false;
    }
    printf("test bench 1 passed!\n");
    
    // run test bench 2
    for (uint32_t i = 0; i < reads.size(); i++) {
      uint32_t sa_size = reads[i].high - reads[i].low + 1;
      for (uint32_t j = 0; j < sa_size; j++) {
	char tmp[MAX_READ_LENGTH+1] = {0};
	uint32_t pos = sai[reads[i].low + j];
	strncpy(tmp, &ref[pos], reads[i].len);
	if (strcmp(reads[i].sym, tmp) != 0) {
	  printf("test bench 2 failed!\n");
	  exit(1);
	}
      }
    }
    printf("test bench 2 passed!\n");
  }
  
  // cleanup
  delete[] idx;
  delete[] sai;
  delete[] ref;
  
  return 0;
}

template<typename T> void allocate(T * &a, uint64_t n)
{
  a = new T [n/sizeof(T)];
  if (!a) {
    printf("error: unable to allocate memory!\n");
    exit(1);
  }
}
