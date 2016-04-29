/* reads.hpp ----- James Arram 2014 */

#ifndef READS_H
#define READS_H

#include <stdio.h>
#include <cstdlib>
#include <stdint.h>
#include <string.h>
#include <omp.h>
#include <vector>
#include "def.hpp"
#include <sys/time.h>

// read
struct read_t {
  char sym[MAX_READ_LENGTH+1];
  uint8_t pck_sym[CEIL(MAX_READ_LENGTH, 4)];
  uint32_t low;
  uint32_t high;
  uint8_t len;
  bool is_align;
};

// load reads
void loadReads(FILE *fp, std::vector<read_t> &reads);

#endif