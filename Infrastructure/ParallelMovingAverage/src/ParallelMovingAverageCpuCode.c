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
  int *b = malloc(sizeof(int) * inSize);
  int *expected = malloc(sizeof(int) * inSize);
  int *out = malloc(sizeof(int) * inSize);
  memset(out, 0, sizeof(int) * inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected[i] = 2 * i;
  }

  printf("Running on DFE.\n");

  const int numEngines = 1;
  ParallelMovingAverage_actions_t *actions[numEngines];
  for (int i = 0; i < numEngines; i++) {
    actions[i] = malloc(sizeof(ParallelMovingAverage_actions_t));
    actions[i]->param_N = inSize;
    actions[i]->instream_a = a;
    actions[i]->instream_b = b;
    actions[i]->outstream_output = out;
  }

  max_file_t *maxfile = ParallelMovingAverage_init();
  max_group_t *group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "1", numEngines);

  ParallelMovingAverage_run_group(group, actions[0]);
  //  ParallelMovingAverage_run_group(group, actions[1]);

  max_unload_group(group);

  max_file_free(maxfile);

  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
             i, out[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
