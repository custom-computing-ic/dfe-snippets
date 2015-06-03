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

void run(int mult);

int main(void) {
	for (int i = 2; i < 256; i *= 2)
		run(i);
	return 0;
}

void run(int mult) {

  const int inSize = 384;

  vector<int> a(mult * inSize),  expected(inSize), out(inSize, 0);

  for(int i = 0; i < mult * inSize; ++i) {
    a[i] = i;
  }

  for(int i = 0; i < inSize; ++i) {
    expected[i] = i;
  }

  cout << "Writing to DFE Memory...";
  TerminationCondition_writeDRAM(mult * inSize * sizeof(int), 0, (uint8_t*)&a[0]);
  cout << "done" << endl;

  cout << "Running on DFE. Queued " << mult << " bursts." << endl;
  TerminationCondition(inSize, &out[0], 0, mult * inSize * sizeof(int));

  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected[i]);
      exit(1);
    }

  cout << "Test passed!" << endl;
}
