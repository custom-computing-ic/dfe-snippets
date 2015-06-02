/***
    Runs multiple bitonic sorting kernels and does output merging
    using a min heap.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <queue>

using namespace std;

typedef pair<int, int> pii;

int cmp(const void *e1, const void *e2) {
  int f1 = *((int *)e1);
  int f2 = *((int *)e2);
  if (f1 > f2) return 1;
  if (f1 < f2) return -1;
  return 0;
}

void print_vec(int *vec, int size) {
  for(int i = 0; i < size / BitonicSorting_networkWidth; ++i) {
    for (int j = 0 ; j < BitonicSorting_networkWidth; j++)
      printf("%d ", vec[i * BitonicSorting_networkWidth + j]);
    printf("\n");
  }
  printf("%d\n", vec[size - 1]);
}

int main(void) {

  int networkWidth = BitonicSorting_networkWidth;
  int chunks = 384;
  int inSize = networkWidth * chunks;

  int *in_array = (int *)malloc(sizeof(int) * inSize);
  int *expected = (int *)malloc(sizeof(int) * inSize);
  int *out_array = (int *)malloc(sizeof(int) * inSize);

  for (int i = 0; i < inSize; i++)
    expected[i] = in_array[i] = rand();

  qsort(expected, inSize, sizeof(int), cmp);

  printf("\nRunning on DFE.\n");
  char *dfeIds[] = {"1", "2", "3", "4"};

  max_file_t *maxfile = BitonicSorting_init();

  int numEngines = 2;

#pragma omp parallel for
  for (int i = 0; i < numEngines; i++) {
    BitonicSorting_actions_t actions;
    actions.param_N = inSize / numEngines;
    int offset = i * inSize / numEngines;
    actions.instream_in_array = in_array + offset;
    actions.outstream_out_array = out_array + offset;

    char id[100];
    strncpy(id, "local:", 100);
    strcat(id, dfeIds[i]);

    max_engine_t *engine = max_load(maxfile, id);
    BitonicSorting_run(engine, &actions);
    max_unload(engine);
  }

  // Merge "chunks" sorted streams using a min heap
  // each entry contains a value and the index at which it's found
  priority_queue<pii, vector<pii>, greater<pii> > min_heap;
  for (int i = 0; i < chunks; i++) {
    int idx = i * networkWidth;
    min_heap.push(make_pair(out_array[idx], idx));
  }

  int *new_out = (int *)malloc(sizeof(int) * inSize);
  for (int i = 0; i < inSize; i++) {
    pair<int, int> entry = min_heap.top(); min_heap.pop();
    new_out[i] = entry.first;
    if ((entry.second % networkWidth) == networkWidth - 1)
      continue;
    entry.second++;
    int val = out_array[entry.second];
    min_heap.push(make_pair(val, entry.second));
  }

  for (int i = 0; i < inSize; i++)
    if (new_out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %i : %d != %d\n",
             i, new_out[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
