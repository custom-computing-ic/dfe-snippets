/***
    This project performs a naive matrix multiplication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
//#include "Maxfiles.h"
//#include "MaxSLiCInterface.h"

// two 24 x 24 matrices = 576 ints = 6 * 384 bytes; this constant is
// defined in the Manager, to make sure the CPU and DFE are in sync
const int inSize = 256;

static char IMG1_Path[50] = "../src/imgs/i1.txt";
static char IMG2_Path[50] = "../src/imgs/i2.txt";
static char IMG3_Path[50] = "../src/imgs/i3.txt";
static char IMG4_Path[50] = "../src/imgs/i4.txt";

void print_matrix(float *mat, int size) {
  for(int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j)
      printf("%f ", mat[i * inSize + j]);
    printf("\n");
  }
}


int main(void)
{

  int img_num = 1;
  int overhead = 131328 + 286 + 3 + 542 + 65536 + 286 + 3 + 542 + 286 + 3 + 542 + 65536 + 286 + 3 + 542; //+ 286 + 3 + 542 + 65536 + 286 + 3 + 542; //+3 to make it multiple of 16 bits
  //int overhead = 0;
  /*
  int *i1 = malloc (sizeof(int) * inSize * (inSize+overhead));
  int *i2 = malloc (sizeof(int) * inSize * (inSize+overhead));
  int *i3 = malloc (sizeof(int) * inSize * (inSize+overhead));
  int *i4 = malloc (sizeof(int) * inSize * (inSize+overhead));
  */
  
  // Read i1, i2, i3, i4
  
  float *i1 = (float *) malloc (sizeof(float) * (inSize * inSize * img_num + overhead));
  float *i2 = (float *) malloc (sizeof(float) * (inSize * inSize * img_num + overhead));
  float *i3 = (float *) malloc (sizeof(float) * (inSize * inSize * img_num + overhead));
  float *i4 = (float *) malloc (sizeof(float) * (inSize * inSize * img_num + overhead));
  memset(i1, 0, sizeof(float) * (inSize * inSize+overhead));
  memset(i2, 0, sizeof(float) * (inSize * inSize+overhead));
  memset(i3, 0, sizeof(float) * (inSize * inSize+overhead));
  memset(i4, 0, sizeof(float) * (inSize * inSize+overhead));
  
  FILE *IMG1_FilePtr;
  FILE *IMG2_FilePtr;
  FILE *IMG3_FilePtr;
  FILE *IMG4_FilePtr;
  IMG1_FilePtr = fopen(IMG1_Path, "r");
  IMG2_FilePtr = fopen(IMG2_Path, "r");
  IMG3_FilePtr = fopen(IMG3_Path, "r");
  IMG4_FilePtr = fopen(IMG4_Path, "r");
  
  printf("Reading images matrix from files.\n"); 
  if (IMG1_FilePtr == NULL | IMG2_FilePtr == NULL | IMG3_FilePtr == NULL | IMG4_FilePtr == NULL) {
      printf("Error opening image file!\n");
  }
  
  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      if (fscanf(IMG1_FilePtr, "%f ", &i1[j*256+i]) == EOF)
        break;
    }
    fscanf(IMG1_FilePtr, "\n");
  }
 
  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      if (fscanf(IMG2_FilePtr, "%f ", &i2[j*256+i]) == EOF)
        break;
    }
    fscanf(IMG2_FilePtr, "\n");
  }

  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      if (fscanf(IMG3_FilePtr, "%f ", &i3[j*256+i]) == EOF)
        break;
    }
    fscanf(IMG3_FilePtr, "\n");
  }

  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      if (fscanf(IMG4_FilePtr, "%f ", &i4[j*256+i]) == EOF)
        break;
    }
    fscanf(IMG4_FilePtr, "\n");
  }
  
  fclose(IMG1_FilePtr);
  fclose(IMG2_FilePtr);
  fclose(IMG3_FilePtr);
  fclose(IMG4_FilePtr);

  printf("Duplicating images for testing.\n");
  //replicate img1, 2, 3, 4 for test
  for (int img_count = 1; img_count < img_num; img_count++)
    for (int i = 0; i < 256; i++)
      for (int j = 0; j < 256; j++)
      {
        i1[65536*img_count + i*256 + j] = i1[i*256 + j];
        i2[65536*img_count + i*256 + j] = i2[i*256 + j];
        i3[65536*img_count + i*256 + j] = i3[i*256 + j];
        i4[65536*img_count + i*256 + j] = i4[i*256 + j];
      }



  /*
  float *i1_bgs = malloc (sizeof(float) * inSize * inSize);
  float *i1_bgs_positive = malloc (sizeof(float) * inSize * inSize);  
  float *i1_norm = malloc (sizeof(float) * inSize * inSize);

  float *i2_bgs = malloc (sizeof(float) * inSize * inSize);
  float *i2_bgs_positive = malloc (sizeof(float) * inSize * inSize);
  float *i2_norm = malloc (sizeof(float) * inSize * inSize);

  float *i3_bgs = malloc (sizeof(float) * inSize * inSize);
  float *i3_bgs_positive = malloc (sizeof(float) * inSize * inSize);
  float *i3_norm = malloc (sizeof(float) * inSize * inSize);

  float *i4_bgs = malloc (sizeof(float) * inSize * inSize);
  float *i4_bgs_positive = malloc (sizeof(float) * inSize * inSize);
  float *i4_norm = malloc (sizeof(float) * inSize * inSize);


  int *expected = malloc (sizeof(int) * inSize * inSize);
  float *mean1 = malloc (sizeof(float) * (inSize));
  float *mean2 = malloc (sizeof(float) * (inSize));
  float *mean3 = malloc (sizeof(float) * (inSize));
  float *mean4 = malloc (sizeof(float) * (inSize));
  float *i1_bgs_newMin = malloc(sizeof(float) * 4); 
  float *i1_pos_newMax = malloc(sizeof(float) * 4);

  float *i2_bgs_newMin = malloc(sizeof(float) * 4);
  float *i2_pos_newMax = malloc(sizeof(float) * 4);

  float *i3_bgs_newMin = malloc(sizeof(float) * 4);
  float *i3_pos_newMax = malloc(sizeof(float) * 4);

  float *i4_bgs_newMin = malloc(sizeof(float) * 4);
  float *i4_pos_newMax = malloc(sizeof(float) * 4);

  float *i6 = malloc (sizeof(float) * inSize * inSize);
  float *i8 = malloc (sizeof(float) * inSize * inSize);
  float *theta_y = malloc (sizeof(float) * inSize * inSize);
  float *dphi_y = malloc (sizeof(float) * inSize * inSize);
  float *G = malloc(sizeof(float) * inSize * inSize *2);
  */
  float *phi = (float *) malloc(sizeof(float) * inSize * inSize * img_num);
  //float *phi = (float *) malloc(sizeof(float) * inSize * inSize);
  /*
  memset(i1, 0, sizeof(int) * inSize * (inSize+overhead));
  memset(i2, 0, sizeof(int) * inSize * (inSize+overhead));
  memset(i3, 0, sizeof(int) * inSize * (inSize+overhead));
  memset(i4, 0, sizeof(int) * inSize * (inSize+overhead));
  */
  /*
  memset(i1_bgs, 0, sizeof(float) * inSize * inSize);
  memset(i1_bgs_positive, 0, sizeof(float) * inSize * inSize);
  memset(i1_norm, 0, sizeof(float) * inSize * inSize);
  
  memset(i2_bgs, 0, sizeof(float) * inSize * inSize);
  memset(i2_bgs_positive, 0, sizeof(float) * inSize * inSize);
  memset(i2_norm, 0, sizeof(float) * inSize * inSize);

  memset(i3_bgs, 0, sizeof(float) * inSize * inSize);
  memset(i3_bgs_positive, 0, sizeof(float) * inSize * inSize);
  memset(i3_norm, 0, sizeof(float) * inSize * inSize);

  memset(i4_bgs, 0, sizeof(float) * inSize * inSize);
  memset(i4_bgs_positive, 0, sizeof(float) * inSize * inSize);
  memset(i4_norm, 0, sizeof(float) * inSize * inSize);


  memset(mean1, 0, sizeof(float) * (inSize));
  memset(mean2, 0, sizeof(float) * (inSize));
  memset(mean3, 0, sizeof(float) * (inSize));
  memset(mean4, 0, sizeof(float) * (inSize));
  memset(i1_bgs_newMin, 0, sizeof(float) * 4);
  memset(i1_pos_newMax, 0, sizeof(float) * 4);

  memset(i2_bgs_newMin, 0, sizeof(float) * 4);
  memset(i2_pos_newMax, 0, sizeof(float) * 4);

  memset(i3_bgs_newMin, 0, sizeof(float) * 4);
  memset(i3_pos_newMax, 0, sizeof(float) * 4);

  memset(i4_bgs_newMin, 0, sizeof(float) * 4);
  memset(i4_pos_newMax, 0, sizeof(float) * 4);

  memset(expected, 0, sizeof(int) * inSize  * inSize);
  */
  // if you want deterministic random numbers, comment out the line below
  /*
  srand(time(NULL));
  for (int k = 0; k < img_num; ++k)
    for(int i = 0; i < inSize; ++i) {
      for (int j = 0; j < inSize; ++j) {
        i1[k * inSize * inSize + i * inSize + j] = -rand() % 100+1;
        i2[k * inSize * inSize + i * inSize + j] = -rand() % 100;
        i3[k * inSize * inSize + i * inSize + j] = -rand() % 100;
        i4[k * inSize * inSize + i * inSize + j] = -rand() % 100;
      }
    }
  */
  /* 
  printf("Matrix i1:\n");
  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
      printf("%f ", i1[i*256+j]);
    printf("\n");
  }
  
  printf("\nMatrix i2:\n");
  print_matrix(i2, inSize);

  printf("\nMatrix i3:\n");
  print_matrix(i3, inSize);

  printf("\nMatrix i4:\n");
  print_matrix(i4, inSize);
  */ 
  printf("Running on FPGA. \n");
  

  //invoke FPGA function
  /*
  QPI(inSize * (inSize+overhead) , i1, i2, i3, i4, 
      dphi_y,
      i1_bgs, 
      //i1_bgs_newMin, 
      i1_bgs_positive,
      i1_norm,
      //i1_pos_newMax,
      i2_bgs_positive,
      i2_norm,
      i6,
      //i8,
      //mean1, mean2, mean3, mean4
      theta_y
      );
   */
  //QPI for assembly
    printf("Number of images = %d\n", img_num);
    struct timeval time0;
    struct timeval time1;
    gettimeofday(&time0, NULL);
  
    QPI((inSize * inSize * img_num + overhead), img_num, i1, i2, i3, i4, phi);

    gettimeofday(&time1, NULL);
    printf("Total DFE Time = %f\n", (time1.tv_sec - time0.tv_sec)+(time1.tv_usec - time0.tv_usec)*1e-6);
    
  //QPI(1640, i1, i2, i3, i4, dphi_y, i1_bgs_positive, i1_norm, i2_bgs_positive, i2_norm, i6, i8, theta_y);

  /*
  for (int i = 0; i < inSize; i++) {
      printf("mean1[%d] = %f\n", i, mean1[i]);
      printf("mean2[%d] = %f\n", i, mean2[i]);
      printf("mean3[%d] = %f\n", i, mean3[i]);
      printf("mean4[%d] = %f\n", i, mean4[i]);
}


  printf("\nbackground subtraction image1: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i1_bgs[i * inSize + j]);
    printf("\n");
  }



  ////compare CPU min and FPGA min  
  float CPU_min = 0;
  for (int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j) {
  if (i1_bgs[i*inSize+j] < CPU_min) CPU_min = i1_bgs[i*inSize+j];
    }
  }

  printf("\nig_bgs_newMin = %f \n", i1_bgs_newMin[3]);
  printf("\nCPU_min = %f \n\n", CPU_min);


  printf("\nbackground subtraction image1 positive: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i1_bgs_positive[i * inSize + j]);
    printf("\n");
  }

  float CPU_max_1 = 0;
  for (int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j) {
        if (i1_bgs_positive[i*inSize+j] > CPU_max_1) CPU_max_1 = i1_bgs_positive[i*inSize+j];
    }
  }

  printf("\ni1_pos_newMax = %f \n", i1_pos_newMax[3]);
  printf("\nCPU_max_1 = %f \n\n", CPU_max_1);

  printf("\nimage1 normalization: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i1_norm[i * inSize + j]);
    printf("\n");
  }

  printf("\nbackground subtraction image2 positive: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i2_bgs_positive[i * inSize + j]);
    printf("\n");
  }

  float CPU_max_2 = 0;
  for (int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j) {
        if (i2_bgs_positive[i*inSize+j] > CPU_max_2) CPU_max_2 = i2_bgs_positive[i*inSize+j];
    }
  }

  printf("\ni2_pos_newMax = %f \n", i2_pos_newMax[3]);
  printf("\nCPU_max_2 = %f \n\n", CPU_max_2);

  printf("\nimage2 normalization: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i2_norm[i * inSize + j]);
    printf("\n");
  }

  printf("\ni6: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i6[i * inSize + j]);
    printf("\n");
  }

  printf("\ni8: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", i8[i * inSize + j]);
    printf("\n");
  }

  printf("\ntheta_y: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", theta_y[i * inSize + j]);
    printf("\n");
  }

  printf("\ndphi_y: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", dphi_y[i * inSize + j]);
    printf("\n");
  }
  */
  /*
  printf("G:\n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize*2 ; j+=2)
      printf("G[%d,%d] = %f + i %f   ", i, j/2, G[i * inSize + j], G[i * inSize+j+1]);
    printf("\n");
  }
  */
    
  printf("\nphi: \n\n");
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j)
      printf("%f ", phi[65536*(img_num-1) + i * inSize + j]);
    printf("\n");
  }
  

  //printf("Shutting down.\n");
  free(i1);
  free(i2);
  free(i3);
  free(i4);
  free(phi);
  return 0;
}


