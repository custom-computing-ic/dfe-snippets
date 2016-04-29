/* file.cpp ----- James Arram 2015 */

#include "file.hpp"

// open file 
void openFile(FILE **fp, char *f_name, const char *mode)
{
  *fp = fopen(f_name, mode);
  if (!(*fp)) {
    printf("error: unable to open file '%s'!\n", f_name);
    exit(1);
  }
}

// get file size in bytes
uint64_t fileSizeBytes(FILE *fp)
{
  struct stat st;
  uint64_t len;
  int fd;

  if ((fd = fileno(fp)) == -1) {
    printf("error: unable to get file size!\n");
    exit(1);
  }

  if(fstat(fd, &st)) { 
    printf("error: unable to get file size!\n"); 
    exit(1);
  } 
  
  len = st.st_size;

  return len;
}

// read file
void readFile(FILE *fp, void *a, uint64_t n_bytes)
{
  if (fread(a, n_bytes, 1, fp) != 1) {
    printf("error: unable to read file!\n");
    exit(1);
  }
}

// write file
void writeFile(FILE *fp, void *a, uint64_t n_bytes)
{
  if (fwrite(a, n_bytes, 1, fp) != 1) {
    printf("error: unable to write file!\n");
    exit(1);
  }
}




 
