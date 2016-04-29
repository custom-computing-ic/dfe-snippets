/* reads.cpp ----- James Arram 2015 */

#include "reads.hpp"

// pack symbols                                    
void packSymbols(char *sym, uint8_t *pck, uint8_t len);

// set value in packed read
inline void setVal(uint8_t *pck, uint32_t idx, uint8_t val);

// load reads
void loadReads(FILE *fp, std::vector<read_t> &reads)
{
  read_t tmp;
  
  tmp.is_align = false;

  // read fastq file
  while (fscanf(fp, "%*[^\n]\n%s\n%*[^\n]\n%*[^\n]\n", tmp.sym) == 1) {
    tmp.len = strlen(tmp.sym);      
    reads.push_back(tmp);
  }

  // pack read symbols
#pragma omp parallel for num_threads(N_THREADS)
  for (uint32_t i = 0; i < reads.size(); i++) {
    memset(reads[i].pck_sym, 0, CEIL(reads[i].len, 4)*sizeof(uint8_t));
    packSymbols(reads[i].sym, reads[i].pck_sym, reads[i].len);
  }
}

// pack symbols                                    
void packSymbols(char *sym, uint8_t *pck, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++) {
    switch(sym[len-1-i]) {
    case 'A': setVal(pck, i, 0); break;
    case 'C': setVal(pck, i, 1); break;
    case 'G': setVal(pck, i, 2); break;
    case 'T': setVal(pck, i, 3); break;
    default : setVal(pck, i, 0); 
    }
  }
}

// set value in packed read
inline void setVal(uint8_t *pck, uint32_t idx, uint8_t val)
{
  uint8_t tmp = val << ((idx*2)%8);
  pck[idx/4] |= tmp;
}