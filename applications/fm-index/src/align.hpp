/* align.hpp ----- James Arram 2014 */

/*
 * align header file
 */

#ifndef ALIGN_H
#define ALIGN_H

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <stdint.h> 
#include <sys/time.h>
#include <omp.h>
#include <vector>
#include <MaxSLiCInterface.h>
#include "def.hpp"
#include "reads.hpp"
#include "index.hpp"
#include "Em.h"

// align reads
void align(std::vector<read_t> &reads, index_t *index, uint64_t index_bytes, 
	   uint32_t high_init);

#endif