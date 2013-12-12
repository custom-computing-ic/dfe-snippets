/* MAXELER MEMORY TESTER
 *
 * < Host code >
 * Host signal and control. You may edit user parameters at the top to test 
 * performance using different settings.
 *
 * Written by Kit Cheung, Imperial College London */


#include <stdlib.h>
#include <string.h>
#include "MaxSLiCInterface.h"
#include "mem_test.h"


// User parameters (no hardware reconfig required)
#define MEM_TO_MEM true			// Write content back to LMEM
#define MEM_TO_HOST false		// Send LMEM content to host
#define RAND_ACCESS true		// Random LMEM memory access addresses
#define MEM_STREAMS 1			// Number of parallel command / memory streams to use
#define BURSTS_PER_CMD 128		// Number of requested bursts per command
#define DRAM_BURSTS 600000000	// Total size of data to read from LMEM in bursts


// Constants, don't change
#define BURST_SIZE 384		// Memory burst size in bytes
#define MAX_ACCESS 65536	// Max number of different memory space to access,
							// limited by size of mapped ROM (used to store addresses)


char* strcat_int(const char *str_in, int i){
	char *str, str_c[2];
	str = (char*)malloc(30);
	strcpy(str, str_in);
	sprintf(str_c, "%d", i);
	strcat(str, str_c);
	return str;
}


char* strcat_str(char *str_in1, const char *str_in2){
	char *str;
	str = (char*)malloc(50);
	strcpy(str, str_in1);
	strcat(str, str_in2);
	return str;
}


int main(int argc, char *argv[]) {

	// Host
	FILE *f;
	int max_streams, mem_stm_width, lmem_linear_access;
	int bursts_per_stream = (int)(DRAM_BURSTS/BURSTS_PER_CMD/MEM_STREAMS)*BURSTS_PER_CMD;
	int access_count = bursts_per_stream/BURSTS_PER_CMD;
	bool MAIA = argv[2][0] - 48; // automatic host detection
	struct timeval tv1, tv2;
	char *str;
	uint64_t i;
	int j;
	double runtime;
	uint8_t* mem_data = (uint8_t*)malloc((long)bursts_per_stream * BURST_SIZE);
	max_file_t *maxfile = mem_test_init();
	max_engine_t *engine;
	max_actions_t *action = max_actions_init(maxfile, NULL);

	// Read system config file
	f = fopen(strcat_str(argv[1], "/config.txt"), "r");
	fscanf(f, "%d %d %d", &max_streams, &mem_stm_width, &lmem_linear_access);
	fclose(f);

	// Array to store memory access addresses
	uint64_t *dram_addrR = (uint64_t*)malloc(max_streams*MAX_ACCESS*sizeof(uint64_t));
	uint64_t *dram_addrW = (uint64_t*)malloc(max_streams*MAX_ACCESS*sizeof(uint64_t));

	// Scalar input
	int route_kernel_ticks = (long)bursts_per_stream * BURST_SIZE / (mem_stm_width / 8);
	max_set_uint64t(action, "RouteKernel", "mem_to_mem", MEM_TO_MEM);
	max_set_uint64t(action, "RouteKernel", "mem_to_host", MEM_TO_HOST);
	max_set_uint64t(action, "RouteKernel", "ticks", route_kernel_ticks);
	max_set_uint64t(action, "RouteKernel", "mem_streams", MEM_STREAMS-1);
	max_set_ticks(action, "RouteKernel", route_kernel_ticks);

	// Since custom command must be separeted by at least 1 cycle (MAX4), so at least
	// (access_count*2) ticks for CmdKernel
	max_set_ticks(action, "CmdKernel", access_count*2);

	// Error checking
	if (MEM_STREAMS > max_streams){
		printf("[ERROR] MEM_STREAM must be smaller or equal to max_streams\n");
		return 0;
	}
	else if (MEM_STREAMS == 0){
		printf("[ERROR] MEM_STREAM must be >= 1\n");
		return 0;
	}

	if (lmem_linear_access){
		for (i = 0; i < max_streams; i++){
			if (i < MEM_STREAMS){
				str = strcat_int("from_dram", i);
				max_lmem_linear_advanced(action, str, 0, 65536*BURST_SIZE,
						(long)bursts_per_stream*BURST_SIZE, 0);

				str = strcat_int("to_dram", i);
				max_lmem_linear_advanced(action, str, 65536*BURST_SIZE, 65536*BURST_SIZE,
						(long)bursts_per_stream*BURST_SIZE, 0);
			}
			else {
				str = strcat_int("from_dram", i);
				max_ignore_lmem(action, str);
				str = strcat_int("to_dram", i);
				max_ignore_lmem(action, str);
			}
		}
	}
	else {
		// Scalar input (Command Kernel)
		max_set_uint64t(action, "CmdKernel", "mem_to_mem", MEM_TO_MEM);
		max_set_uint64t(action, "CmdKernel", "burst_size", BURSTS_PER_CMD);
		max_set_uint64t(action, "CmdKernel", "dram_bursts", (long)bursts_per_stream);
		max_set_uint64t(action, "CmdKernel", "mem_streams", MEM_STREAMS-1);

		// Mapped ROM for address access sequence
		for (i = 0; i < max_streams; i++){
			for (j = 0; j < MAX_ACCESS; j++){
				// Random addresses or sequential addresses (for mem command only)
				if (RAND_ACCESS){
					dram_addrR[i*MAX_ACCESS+j] = rand() % (1 << 26);
					dram_addrW[i*MAX_ACCESS+j] = rand() % (1 << 26);
				}
				else{
					dram_addrR[i*MAX_ACCESS+j] = j;
					dram_addrW[i*MAX_ACCESS+j] = j;
				}

				str = strcat_int("dram_addrR", i);
				max_set_mem_uint64t(action, "CmdKernel", str, j, dram_addrR[i*MAX_ACCESS+j]);

				str = strcat_int("dram_addrW", i);
				max_set_mem_uint64t(action, "CmdKernel", str, j, dram_addrW[i*MAX_ACCESS+j]);
			}
		}
	}


	// PCI to host
	if (MEM_TO_HOST){
		max_queue_output(action, "to_host", mem_data, (long)bursts_per_stream * BURST_SIZE);
	}
	else {
		// Even when no data is needed to send to host, 1 burst is sent to host for the kernel
		// to signal its finish
		max_queue_output(action, "to_host", mem_data, 16);
	}

	if (!MAIA){
		engine = max_load(maxfile, "local");
	}
	else {
		engine = max_load(maxfile, "192.168.0.10");
	}
	

	// Run the DFE
	gettimeofday(&tv1, NULL);
	max_run(engine, action);
	gettimeofday(&tv2, NULL);
	runtime = (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
	if (MAIA){
		runtime -= 150000;
	}
	else {
		runtime -= 76000;
	}
	max_actions_free(action);
	max_unload(engine);

	max_file_free(maxfile);	


	// Calculate performance
	int total_streams = MEM_STREAMS * (MEM_TO_MEM+1);
	printf(">> Test completed!\n");
	printf(">> Runtime: %0.3fs\n", (runtime/1000000));
	printf(">> Transfer Speed: %0.3fGB/s x %d = %0.3fGB/s\n", 
			(long)bursts_per_stream*BURST_SIZE/runtime/1000, total_streams,
			(long)bursts_per_stream*BURST_SIZE/runtime/1000 * total_streams);
	
	return 0;
}
