#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"
#include "OptionPricing.hpp"

#define real double
#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

double optionPricing(double strike, double sigma, double timestep, int numMaturity,
                     int paraNode, int numPathGroup, double T,
                     double **out_rand1, double **out_rand2,
                     int numEngines
                     ) {

  //need to change the value in the java file
  int numPE = 4;

  printf("--> Bitstream::optionPricing(numMaturity=%d, numPathGroup=%d)\n",
	 numMaturity, numPathGroup);

  real *maturity = (real *)malloc(sizeof(real) * numPE * numMaturity);
  real *maturity_diff = (real *)malloc(sizeof(real) * numPE * numMaturity);
  real *f = (real *)malloc(sizeof(real) * numPE * numMaturity);
  real *fout = (real *)malloc(sizeof(real) * 2048);
  int seedsize = 64 * 100;
  unsigned int *seed1 = (unsigned int *)malloc(sizeof(unsigned int) * seedsize);
  unsigned int *seed2 = (unsigned int *)malloc(sizeof(unsigned int) * seedsize);
  int i, j, k;
  for( i = 0; i < 64; i++){
    seed1[i] = i;
    seed2[i] = 7-i;
  }

  for(i=0;i<16;i++){
    for(j=0;j<numPE;j++){
      seed1[i*4*numPE+j*4] = i*4;
      seed1[i*4*numPE+j*4+1] = i*4+1;
      seed1[i*4*numPE+j*4+2] = i*4+2;
      seed1[i*4*numPE+j*4+3] = i*4+3;
      seed2[i*4*numPE+j*4]   = 7- i*4;
      seed2[i*4*numPE+j*4+1] = 7-(i*4+1);
      seed2[i*4*numPE+j*4+2] = 7-(i*4+2);
      seed2[i*4*numPE+j*4+3] = 7-(i*4+3);
    }
  }

  for(j=0;j<numPE;j++){
    maturity[0*numPE+j] = 0;
    maturity[1*numPE+j] = 1;
    maturity[2*numPE+j] = 3;
    maturity[3*numPE+j] = 5;
    maturity[4*numPE+j] = 10;
    maturity[5*numPE+j] = 30;

    f[0*numPE+j] = 0.0695;
    f[1*numPE+j] = 0.075;
    f[2*numPE+j] = 0.078;
    f[3*numPE+j] = 0.0797;
    f[4*numPE+j] = 0.082;
    f[5*numPE+j] = 0.086;
    f[6*numPE+j] = 0.086;
    f[7*numPE+j] = 0.086;
  }

  for(j = 0; j<numMaturity-1; j++){
    for(k=0;k<numPE;k++){
      maturity_diff[j*numPE+k] = 0;
      if(j>0)
        maturity_diff[j*numPE+k] = maturity[j*numPE+k]- maturity[(j-1)*numPE+k];
    }
  }

  int outputRand = 1;
  int initMax = 8;
  real* rand1 = (real*)malloc(numPE*(numPathGroup)*paraNode*sizeof(real));
  real* rand2 = (real*)malloc(numPE*(numPathGroup)*paraNode*sizeof(real));
  //  printf("strike=%f, run for %d times\n", strike, numPathGroup*paraNode);

  if (out_rand1 != NULL) {
    *out_rand1 = rand1;
  }

  if (out_rand2 != NULL) {
    *out_rand2 = rand2;
  }

  OptionPricing_actions_t *actions[numEngines];

#pragma openmp parallel for
  for (int i = 0; i < numEngines; i++) {
    int bytes = sizeof(OptionPricing_actions_t);
    actions[i] = (OptionPricing_actions_t *)malloc(bytes);
    actions[i]->param_initsize = numPE*initMax;
    actions[i]->param_nodesize = numPE * paraNode;
    actions[i]->param_pathsize = numPE * (numPathGroup / numEngines) * paraNode;
    actions[i]->param_seedsize = numPE * 64;

    actions[i]->ticks_OptionPricingKernel = (initMax + (numMaturity-1)*(numPathGroup / numEngines)*paraNode);

    actions[i]->inscalar_OptionPricingKernel_T = T;
    actions[i]->inscalar_OptionPricingKernel_discount = exp(-f[0]*T);
    actions[i]->inscalar_OptionPricingKernel_numMaturity = numMaturity;
    actions[i]->inscalar_OptionPricingKernel_numPath = numPathGroup / numEngines;
    actions[i]->inscalar_OptionPricingKernel_outputRand = outputRand;
    actions[i]->inscalar_OptionPricingKernel_sigma = sigma;
    actions[i]->inscalar_OptionPricingKernel_sqrt_t = sqrt(T);
    actions[i]->inscalar_OptionPricingKernel_strike = strike;

    actions[i]->instream_fin = f;
    actions[i]->instream_maturity = maturity;
    actions[i]->instream_maturity_diff = maturity_diff;

    actions[i]->instream_seed = seed1; // + seedsize / numEngines * i;
    actions[i]->instream_seed2 = seed2; // + seedsize / numEngines * i;

    actions[i]->outstream_randOut = rand1;
    actions[i]->outstream_randOut2 = rand2;

    actions[i]->outstream_result = fout + i * paraNode;
  }

  max_file_t *maxfile = OptionPricing_init();
  // TODO[paul-g] at the moment of writing Maxeler's group API seems
  // buggy, with unjustified performance variations under a large
  // number of requests

  max_group_t *group = max_load_group(maxfile, MAXOS_EXCLUSIVE, "local", numEngines);
  #pragma omp parallel for
    for (int i = 0; i < numEngines; i++)
      OptionPricing_run_group(group, actions[i]);
   max_unload_group(group);

// #pragma omp parallel for
//   for (int i = 0; i < numEngines; i++) {
//     char id[100];
//     sprintf(id, "local:%d", i);
//     max_engine_t *engine = max_load(maxfile, id);
//     OptionPricing_run(engine, actions[i]);
//     max_unload(engine);
//   }

  // free resources
  for (int i = 0; i < numEngines; i++)
    free(actions[i]);

  max_file_free(maxfile);

  real sum = 0;
  for(i = 0; i < paraNode * numEngines; i++)
    sum += fout[i*numPE];

  double dfeResult = sum/(numPathGroup)/paraNode;
  //  printf("result = %lf\n", dfeResult);

  free(maturity);
  free(maturity_diff);
  free(f);
  free(fout);
  free(rand1);
  free(rand2);
  free(seed1);
  free(seed2);

  return dfeResult;
}


double cpuOptionPricing(
                        double strike, double sigma, double timestep, int numMaturity,
                        int paraNode, int numPathGroup, double T,
                        double *rand1, double *rand2)
{
  int numPE = 4;
  real *maturity = (real *)malloc(sizeof(real) * numPE * numMaturity);
  real *maturity_diff = (real *)malloc(sizeof(real) * numPE * numMaturity);
  real *f = (real *)malloc(sizeof(real) * numPE * numMaturity);

  double sum = 0.0;
  int N = (numPathGroup)*paraNode;
  maturity[0] = 0;
  maturity[1] = 1;
  maturity[2] = 3;
  maturity[3] = 5;
  maturity[4] = 10;
  maturity[5] = 30;

  f[0] = 0.0695;
  f[1] = 0.075;
  f[2] = 0.078;
  f[3] = 0.0797;
  f[4] = 0.082;
  f[5] = 0.086;
  f[6] = 0.086;
  f[7] = 0.086;

  for(int j = 0; j<numMaturity-1; j++){
    maturity_diff[j] = 0;
    if(j>0)
      maturity_diff[j] = maturity[j]- maturity[(j-1)];
  }

  for(int k = 0; k< N;k++){
    /*-------------one MC iteration----------------------*/
    real discount =1;
    real swapPrice = 0;
    //at timeline[0], f[0 to numMaturity-1] is observed
    f[0] = 0.0695;
    f[1] = 0.075;
    f[2] = 0.078;
    f[3] = 0.0797;
    f[4] = 0.082;
    f[5] = 0.086;
    //just single jump, no time steps involved
    //for(int i = 1; i< numTimestep+1; i++)
    //{
    real A1 = 0;
    real A2 = 0;
    real Bprev = 0;
    //discount based on short rate up until t-1, constant across all paths
    //discount *= exp(-f[0] * T);
    discount *= exp(-f[0] * T);
    //printf("%f\n", f[(i-1)*numMaturity]*timeline[i]);
    real Z1 = rand1[k*numPE];
    real Z2 = rand2[k*numPE];

    real zeroRate = 0;
    real zeroPrice_i_j = 0;
    real annuity=0;
    for(int j = 0; j<numMaturity-1; j++)
      {
        /*-------------------miu-----------------*/
        //time between to maturity date, i.e. viewed from timeline[i]
        //double timespan = 0;
        //if(j>0)
        //  timespan = maturity[j]- maturity[j-1];
        //double vol1 = fvol1(maturity[j]);
        //double vol2 = fvol2(maturity[j]);

        //use textbook miu for testing, old value overwritten
        //miu[j]= fmiu(maturity[j]);
        real realT = maturity[j]+T;
        real miu= 0.5*sigma*sigma*(realT*realT- maturity[j]*maturity[j]);
        real vol1 = sigma;
        real vol2 = sigma;
        /*------------------------------------*/
        //f_new = f_old+miu*dt+vol*sqrt(dt)*dW
        f[j] = f[j+1]+
          miu + sqrt(T)*(vol1*Z1 + vol2*Z2);
        //printf("f1 = %f, miu = %f, sqrt_T=%f, N1 = %f, N2 = %f, fnew = %f, ftime=%f\n", f[j+1],  miu, sqrt(T), Z1, Z2, f[j], f[j]*maturity_diff[j]);

        zeroRate += f[j]*maturity_diff[j];
        zeroPrice_i_j = exp(-zeroRate);

        //printf("j = %d, Local zero rate is %f,\n", j,zeroRate);
        annuity += zeroPrice_i_j;
        //printf("timespan=%f, miu=%f\n", timespan, miu[j]);
      }
    real swapRate= (1-zeroPrice_i_j)/annuity;
    //printf("Local annuity is %f, swap rate is %f, zeroPrice is %f\n", annuity, swapRate, zeroPrice_i_j);
    real payoff_i = (swapRate-strike)>0? (swapRate-strike)*annuity : 0;
    swapPrice = payoff_i*discount;
    //printf("Local swap rate is %f, pay off is %f, discount is %f, price is %f\n", swapRate, payoff_i, discount, swapPrice);
    //}
    //printf("*******Local swap price[%d] is %f\n", k, swapPrice);
    sum += swapPrice;
  }

  free(maturity);
  free(maturity_diff);
  free(f);
  free(rand1);
  free(rand2);

  return sum/N;
}

// int main(int argc, char *argv[]) {

//   // -- Parameters --
//   double strike = 0.01;
//   double sigma = 0.02;
//   double timestep = 0.05;
//   int numTimeStep = (int)(10/0.05);
//   int numMaturity = 6;
//   int paraNode = 50;
//   int numPathGroup = 10000000;
//   int numPE = 4;
//   double *rand1, *rand2;
//   double T = 10;

//   printf("Running main...\n");

//   struct timeval t1, t2;

//   gettimeofday(&t1, NULL);
//   double dfeResult = optionPricing(strike, sigma, timestep, numMaturity,
//                                    paraNode, numPathGroup, T,
//                                    &rand1, &rand2);
//   gettimeofday(&t2, NULL);
//   long long time = ((t2.tv_sec - t1.tv_sec) * 1E6 + t2.tv_usec - t1.tv_usec) / 1E3;
//   printf("FPGA Took %lld ms\n", time);

//   gettimeofday(&t1, NULL);
//   double cpuResult = cpuOptionPricing(strike, sigma, timestep, numMaturity,
//                                       paraNode, numPathGroup, T,
//                                       rand1, rand2);
//   gettimeofday(&t2, NULL);
//   time = ((t2.tv_sec - t1.tv_sec) * 1E6 + t2.tv_usec - t1.tv_usec) / 1E3;
//   printf("CPU Took %lld ms\n", time);


//   if (fabs(cpuResult - dfeResult) > 1E-6) {
//     printf("Error! Expected: %lf Got: %lf\n", cpuResult, dfeResult);
//     return 1;
//   }

//   return 0;
// }
