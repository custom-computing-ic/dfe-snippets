#include "Maxfiles.h"   
#include <MaxSLiCInterface.h>

float dataIn[] = { 3, 6, 3, 9, 12, 6, 3, 3 };
float expected[] = { 0, 4, 6, 8, 9, 7, 4, 0 };
float dataOut[8];

int main() {

  printf("Running DFE\n");
  TrainingExercise(8, dataIn, dataOut);

  int correct = 1;	

  for (int i = 0; i < 8; i++) {
		if (dataOut[i] != expected[i]) {
			printf("Error: ");
			correct = 0;
		}
    printf("dataOut[%d] = %f\n", i, dataOut[i]);
  }

  return correct == 0 ? 1 : 0;
}

