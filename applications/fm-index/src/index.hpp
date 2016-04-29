/* index.hpp ----- James Arram 2016 */

/*
 * Notes:
 * number of steps for FM-index is 1
 */

#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include "def.hpp"

// index structure
struct index_t {
  uint32_t counters[4];
  uint8_t bwt[CEIL(BUCKET_SIZE*3,8)];
};

#endif