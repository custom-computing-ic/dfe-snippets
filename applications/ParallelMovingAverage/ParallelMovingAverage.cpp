#include <omp.h>
/***
    Run a maxfile in parallel on multiple DFEs using DFE groups and
    OpenMP.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParallelMovingAverage.hpp"
#include "ParallelMovingAverageMaxfiles.h"
#include "MaxSLiCInterface.h"

// Using MaxelerOS managed groups of DFEs
static void MovingAverageDFE_groups(int numEngines,
                             max_file_t* maxfile,
                             ParallelMovingAverage_actions_t** actions)
{
  max_group_t *group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "local", numEngines);

#pragma omp parallel for
  for (int i = 0; i < numEngines; i++)
    ParallelMovingAverage_run_group(group, actions[i]);

  max_unload_group(group);

}

// Using max_load/max_unload to load the bitstream on specific DFEs,
// may be required for more advanced scheduling
static void MovingAverageDFE_custom(int numEngines,
                             max_file_t* maxfile,
                             ParallelMovingAverage_actions_t** actions,
                             char** dfeIds)
{
#pragma omp parallel for
  for (int i = 0; i < numEngines; i++) {
    char id[100];
    strncpy(id, "local:", 100);
    strcat(id, dfeIds[i]);
    max_engine_t *engine = max_load(maxfile, id);
    ParallelMovingAverage_run(engine, actions[i]);
    max_unload(engine);
  }
}


void MovingAverageDFE(int width, int n, int *in, int* out,
		      int numEngines, char** dfeIds, bool useGroups) {
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
    int bytes = sizeof(ParallelMovingAverage_actions_t);
    actions[i] = (ParallelMovingAverage_actions_t *)malloc(bytes);
    actions[i]->param_N = itemsPerDfe;
    actions[i]->instream_a = in + itemsPerDfe * i;
    actions[i]->outstream_output = out + itemsPerDfe * i;
  }

  max_file_t *maxfile = ParallelMovingAverage_init();

  if (useGroups) {
    MovingAverageDFE_groups(numEngines, maxfile, actions);
  } else {
    MovingAverageDFE_custom(numEngines, maxfile, actions, dfeIds);
  }

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

  for (int i = 0; i < numEngines; i++)
    free(actions[i]);

  max_file_free(maxfile);
}

void check_results(int n, int* out, int* exp) {
  for (int i = 1; i < n - 1; i++)
    if (out[i] != exp[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
             i, out[i], exp[i]);
      exit(1);
    }
}

// int main(int argc, char** argv) {

//   int n = 384 * 1000000;
//   const int numEngines = 4;

//   int *a = (int *)calloc(n, sizeof(int));
//   int *out = (int *)calloc(n, sizeof(int));
//   for(int i = 0; i < n; ++i)
//     a[i] = i + 1;
//   int *exp = (int *)calloc(n, sizeof(int));
//   for (int i = 1; i < n - 1; i++)
//     exp[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;

//   char *dfeIds[] = {"0", "1", "2", "3"};
//   printf("Running on DFE with groups.\n");
//   MovingAverageDFE(3, n, a, out, numEngines, dfeIds, true);
//   check_results(n, a, exp);

//   printf("Running on DFE with custom mode.\n");
//   MovingAverageDFE(3, n, a, out, numEngines, dfeIds, false);
//   check_results(n, a, exp);

//   free(a);
//   free(exp);
//   free(out);

//   printf("Test passed!\n");
//   return 0;
// }
