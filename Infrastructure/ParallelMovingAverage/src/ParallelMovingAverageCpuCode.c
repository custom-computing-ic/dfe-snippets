/***
    TODO: Add a descriptive comment!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 384;

  int *a = malloc(sizeof(int) * inSize);
  int *expected = malloc(sizeof(int) * inSize);
  int *out = calloc(inSize, sizeof(int));
  for(int i = 0; i < inSize; ++i)
    a[i] = i + 1;

  for (int i = 1; i < inSize - 1; i++)
    expected[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;

  printf("Running on DFE.\n");

  const int numEngines = 2;
  ParallelMovingAverage_actions_t *actions[numEngines];
  for (int i = 0; i < numEngines; i++) {
    actions[i] = malloc(sizeof(ParallelMovingAverage_actions_t));
    actions[i]->param_N = inSize;
    actions[i]->instream_a = a;
    actions[i]->outstream_output = out;
  }

  max_file_t *maxfile = ParallelMovingAverage_init();
  max_group_t *group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "1", numEngines);

#pragma omp parallel for
  for (int i = 0; i < numEngines; i++)
    ParallelMovingAverage_run_group(group, actions[i]);

  max_unload_group(group);
  max_file_free(maxfile);

  for (int i = 1; i < inSize - 1; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
             i, out[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
