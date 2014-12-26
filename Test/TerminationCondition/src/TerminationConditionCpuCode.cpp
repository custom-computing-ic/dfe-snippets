/***
    Check if blocking call returns even if not all queued data is
    read.
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

using namespace std;

int main(void)
{

  const int inSize = 384;

  vector<int> a(2 * inSize),  expected(inSize), out(inSize, 0);

  for(int i = 0; i < 2 * inSize; ++i) {
    a[i] = i;
  }

  for(int i = 0; i < inSize; ++i) {
    expected[i] = i;
  }

  cout << "Writing to DFE Memory...";
  TerminationCondition_writeDRAM(2 * inSize * sizeof(int), 0, (uint8_t*)&a[0]);
  cout << "done" << endl;

  cout << "Running on DFE." << endl;
  TerminationCondition(inSize, &out[0], 0, 2 * inSize * sizeof(int));

  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected[i]);
      return 1;
    }

  cout << "Test passed!" << endl;
  return 0;
}
