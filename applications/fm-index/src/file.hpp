/* file.hpp ----- James Arram 2015 */

/*
 * file utilities
 */

#ifndef FILE_HPP
#define FILE_HPP

#include <stdio.h>
#include <cstdlib>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// open file 
void openFile(FILE **fp, char *f_name, const char *mode);

// get file size in bytes
uint64_t fileSizeBytes(FILE *fp);

// read file
void readFile(FILE *fp, void  *a, uint64_t size);

// write file
void writeFile(FILE *fp, void *a, uint64_t size);

#endif