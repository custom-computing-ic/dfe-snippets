/***
    Sorts nVectors vectors of networkWidth size each.
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
  int chunks = 12;
  int inSize = networkWidth * chunks;

  int *in_array = (int *)malloc(sizeof(int) * inSize);
  int *expected = (int *)malloc(sizeof(int) * inSize);
  int *out_array = (int *)malloc(sizeof(int) * inSize);

  for (int i = 0; i < inSize; i++)
    expected[i] = in_array[i] = rand();

  qsort(expected, inSize, sizeof(int), cmp);

  printf("\nRunning on DFE.\n");
  BitonicSorting(inSize, in_array, out_array);

  // Merge "chunks" sorted streams using a min heap
  // each entry contains a value and the index at which it's found
  priority_queue<pii, vector<pii>, greater<pii> > min_heap;
  for (int i = 0; i < chunks; i++) {
    int idx = i * networkWidth;
    cout << out_array[idx] << endl;
    min_heap.push(make_pair(out_array[idx], idx));
  }

  int *new_out = (int *)malloc(sizeof(int) * inSize);
  for (int i = 0; i < inSize; i++) {
    pair<int, int> entry = min_heap.top(); min_heap.pop();
    new_out[i] = entry.first;
    if ((entry.second % networkWidth) == networkWidth - 1) {
      cout << "Skipping: " << entry.second << endl;
      continue;
    }
    cout << entry.first << " " << entry.second << endl;
    entry.second++;
    int val = out_array[entry.second];
    cout << "  Adding: " << val << " " << entry.second << endl;
    min_heap.push(make_pair(val, entry.second));
  }

  printf("Input:\n");
  print_vec(in_array, inSize);

  printf("\nExpected (sorted):\n");
  print_vec(expected, inSize);

  printf("\nOutput (sorted):\n");
  print_vec(out_array, inSize);

  printf("\nOutput (merged):\n");
  print_vec(new_out, inSize);

  for (int i = 0; i < inSize; i++)
    if (new_out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %i : %d != %d\n",
             i, new_out[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
