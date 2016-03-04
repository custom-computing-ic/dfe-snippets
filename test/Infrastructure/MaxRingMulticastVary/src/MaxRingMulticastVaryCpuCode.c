#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "MaxSLiCInterface.h"
#include "MaxRingMulticastVary.h"

char* strcat_int(const char *str_in, int i){
	char *str, str_c[2];
	str = (char*)malloc(50);
	strcpy(str, str_in);
	sprintf(str_c, "%d", i);
	strcat(str, str_c);
	return str;
}

int main(void)
{

	const int numTotalData[] = {4, 8, 12, 24, 20, 16};
	// Since data are sent in multiple of 32 bytes, dataPerRound should
	// be a multiple of 8 since each data is 32 bits
	int numTotalDataMax = 0, numTotalDataSum = 0;
	const int dataPerRound = 8;
	const int numFPGAs = 6;
	const bool MAX4 = 1;
	const bool sim = true;
	const int FPGA_id_map[6] = {1, 0, 2, 3, 5, 4};
	char eng_string[50], kernel_name[50];
	uint32_t* data[numFPGAs];
	int i, j, kernel_ticks;
	struct timeval tv1, tv2;
	const int dataSize = sizeof(uint32_t); // 4 bytes


	for (i = 0; i < numFPGAs; i++){
		data[i] = (uint32_t*)malloc(numTotalData[i]*numFPGAs*dataSize);
		if (numTotalData[i] > numTotalDataMax){
			numTotalDataMax = numTotalData[i];
		}
		numTotalDataSum += numTotalData[i];
	}

	if (MAX4){
		strcpy(eng_string, "192.168.0.10");
	}
	else{
		strcpy(eng_string, "local");
	}

	for (i = 0; i < numFPGAs; i++){
		for (j = 0; j < numTotalData[i]*numFPGAs; j++){
			data[i][j] = 0;
		}
	}


	max_file_t* maxfile = MaxRingMulticastVary_init();

	if (!sim){
		max_actarray_t *action_array = max_actarray_init(maxfile, numFPGAs);
		max_actions_t *action[numFPGAs];
		max_engarray_t* engine_array = 
			max_load_array(maxfile, numFPGAs, eng_string);

		strcpy(kernel_name, "MaxRingMulticastVaryKernel");
		kernel_ticks = numFPGAs*2 *ceil((float)(numTotalDataMax+1) / dataPerRound) * dataPerRound - dataPerRound;
		for (i = 0; i < numFPGAs; i++){
			action[i] = max_actions_init(maxfile, NULL);
			max_set_uint64t(action[i], kernel_name, "dataPerRound", dataPerRound);
			max_set_uint64t(action[i], kernel_name, "numTotalData", numTotalData[i]);
			max_set_uint64t(action[i], kernel_name, "numFPGAs", numFPGAs);
			max_set_uint64t(action[i], kernel_name, "FPGA_id", i);
			max_set_ticks(action[i], kernel_name, kernel_ticks);
			max_queue_output(action[i], "dataOut", data[i], numTotalDataSum*dataSize);
			max_set_action(action_array, i, action[i]);

		}

		printf(">> Run DFE for %d ticks\n", kernel_ticks);
		gettimeofday(&tv1, NULL);
		max_run_array(engine_array, action_array);
		gettimeofday(&tv2, NULL);
		max_actarray_free(action_array);
		max_unload_array(engine_array);
	}
	else
	{
		max_actions_t *actions = max_actions_init(maxfile, NULL);
		max_engine_t* engine = max_load(maxfile, "local");

		kernel_ticks = numFPGAs*2 *ceil((float)(numTotalDataMax+1) / dataPerRound) * dataPerRound - dataPerRound;
		for (i = 0; i < numFPGAs; i++){
			strcpy(kernel_name, strcat_int("MaxRingMulticastVaryKernel", i));
			max_set_uint64t(actions, kernel_name, "dataPerRound", dataPerRound);
			max_set_uint64t(actions, kernel_name, "numTotalData", numTotalData[i]);
			max_set_uint64t(actions, kernel_name, "numFPGAs", numFPGAs);
			max_set_uint64t(actions, kernel_name, "FPGA_id", i);
			max_set_ticks(actions, kernel_name, kernel_ticks);
			max_queue_output(actions, strcat_int("dataOut", i), data[i], numTotalDataSum*dataSize);
		}
	
		printf(">> Run Sim for %d ticks\n", kernel_ticks);
		gettimeofday(&tv1, NULL);
		max_run(engine, actions);
		gettimeofday(&tv2, NULL);
		max_actions_free(actions);
		max_unload(engine);
	}

	/*
	for (i = 0; i < numFPGAs; i++){
		for (j = 0; j < numTotalData[i]*numFPGAs; j++){
			if (data[i][j] != 0){
				printf("[FPGA%d:%d] Card%u. %u\n", i, j, (data[i][j] / 0x10000000)-8, data[i][j] % 0x10000000);
			}
		}
	}
	*/
	double runtime = ((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0;
	printf(">> Finish!\n");
	printf(">> Runtime: %0.3fs\n", runtime);
}
