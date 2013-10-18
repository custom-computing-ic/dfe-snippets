/**
 * Document: Manager Compiler Tutorial (maxcompiler-manager-tutorial.pdf)
 * Chapter: 2      Example: 1      Name: LMem Example
 * MaxFile name: LMemExample
 * Summary:
 *     Creates a pair of arrays and writes each array to a separate location
 *     in LMem on the DFE. A Kernel running in the DFE reads the arrays from LMem
 *     computes their element-wise sum, and writes the result to a third LMem
 *     location. When this operation is complete, the CPU application reads the
 *     array of sums from the LMem, and verifies that they have been computed
 *     correctly. Groups all memory streams into a single control group.
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <MaxSLiCInterface.h>
#include "Maxfiles.h"

void LMemExampleCPU(int size, int32_t *inA, int32_t *inB, int32_t *outData)
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
	LMemExample_writeLMem(size, 0, inA);
	LMemExample_writeLMem(size, size, inB);

	printf("Running DFE.\n");
	LMemExample(size);

	int32_t *expected = malloc(sizeBytes);
	LMemExampleCPU(size, inA, inB, expected);

	printf("Reading DFE memory.\n");
	int32_t *outData = malloc(sizeBytes);
	LMemExample_readLMem(size, 2 * size, outData);

	int status = check(size, outData, expected);
	if (status)
		printf("Test failed.\n");
	else
		printf("Test passed OK!\n");

	return status;
}
