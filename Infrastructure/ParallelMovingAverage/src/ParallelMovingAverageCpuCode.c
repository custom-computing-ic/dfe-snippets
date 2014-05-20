/***
    Run a maxfile in parallel on multiple DFEs using DFE groups and
    OpenMP.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

// Using MaxelerOS managed groups of DFEs
void MovingAverageDFE_groups(int width, int n, int *in, int* out, int numEngines) {
  if (width != 3) {
    printf("Error, only moving average of width three supported!");
    exit(1);
  }

  if (n % numEngines != 0) {
    printf("Error, stream size must be multiple of number of DFEs!");
    exit(1);
  }

  ParallelMovingAverage_actions_t *actions[numEngines];

  // split data in batches
  int itemsPerDfe = n / numEngines;

  for (int i = 0; i < numEngines; i++) {
    for (int j = 0; j < itemsPerDfe; j++) {
      actions[i] = malloc(sizeof(ParallelMovingAverage_actions_t));
      actions[i]->param_N = itemsPerDfe;
      actions[i]->instream_a = in + itemsPerDfe * i;
      actions[i]->outstream_output = out + itemsPerDfe * i;
    }
  }

  max_file_t *maxfile = ParallelMovingAverage_init();
  max_group_t *group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "1", numEngines);

#pragma omp parallel for
  for (int i = 0; i < numEngines; i++)
    ParallelMovingAverage_run_group(group, actions[i]);

  // need to redo the values around the boundaries between the batches
  for (int i = 0; i < numEngines; i++) {
    int boundary = i * itemsPerDfe;
    for (int j = 0; j < width / 2; j++) {
      int pos1 = boundary + j;
      if (i != 0)
	out[pos1] = (in[pos1 - 1] + in[pos1] + in[pos1 + 1]) / 3;

      int pos2 = boundary + itemsPerDfe - j - 1;
      if (i != numEngines - 1)
	out[pos2] = (in[pos2 - 1] + in[pos2] + in[pos2 + 1]) / 3;
    }

  }

  max_unload_group(group);
  max_file_free(maxfile);

  printf("Test passed!\n");
}

int main(int argc, char** argv) {

  int n = 384;
  const int numEngines = 2;
  int *a = calloc(n, sizeof(int));
  int *out = calloc(n, sizeof(int));
  for(int i = 0; i < n; ++i)
    a[i] = i + 1;

  printf("Running on DFE.\n");
  MovingAverageDFE_groups(3, n, a, out, numEngines);

  // check results
  int *exp = calloc(n, sizeof(int));
  for (int i = 1; i < n - 1; i++)
    exp[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
  for (int i = 1; i < n - 1; i++)
    if (out[i] != exp[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
             i, out[i], exp[i]);
      return 1;
    }

  free(a);
  free(exp);
  free(out);
  return 0;
}
