/*** Linear memory control with support of quarter rate mode */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <MaxSLiCInterface.h>
#include "Maxfiles.h"

void LMemCPU(int size, int32_t *inA, int32_t *inB, int32_t *outData)
{
	for (int i = 0; i < size; i++) {
		outData[i] = inA[i] + inB[i];
	}
}

int check(int size, int32_t *outData, int32_t *expected) {
	int status = 0;
	for (int i = 0; i < size; i++)
		if (outData[i] != expected[i]) {
			fprintf(stderr, "[%d] Verification error, out: %u != expected: %u\n",
				i, outData[i], expected[i]);
			status = 1;
		}
	return status;
}

int main()
{
	const int size = 384;
	int sizeBytes = size * sizeof(int32_t);
	int32_t *inA = malloc(sizeBytes);
	int32_t *inB = malloc(sizeBytes);

	for (int i = 0; i < size; i++) {
		inA[i] = i;
		inB[i] = size - i;
	}

	printf("Loading DFE memory.\n");
	LMem_writeLMem(size, 0, inA);
	LMem_writeLMem(size, size, inB);

	printf("Running DFE.\n");
	LMem(size);

	int32_t *expected = malloc(sizeBytes);
	LMemCPU(size, inA, inB, expected);

	printf("Reading DFE memory.\n");
	int32_t *outData = malloc(sizeBytes);
	LMem_readLMem(size, 2 * size, outData);

	int status = check(size, outData, expected);
	if (status)
		printf("Test failed.\n");
	else
		printf("Test passed OK!\n");

	return status;
}
