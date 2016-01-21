#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

extern void max_reconfig_partial_bitstream(max_engine_t *engine, const char* name);

const char* in_filename = "lena.ppm";
const char* out_filename = "lena_filtered.ppm";
const char* golden_filename = "lena_filtered_golden.ppm";


void loadImage(const char *filename, uint8_t **dest, uint64_t *width_out, uint64_t *height_out)
{
	char buffer[128];
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error opening file %s.", filename);
		exit(1);
	}

	fgets(buffer, sizeof(buffer), file); // Type
	if (strcmp(buffer, "P3\n"))
	{
		printf("Error opening file %s. Only P3 format supported", filename);
		exit(1);
	}

	fgets(buffer, sizeof(buffer), file); // Comment
	int width, height;
	fscanf(file, "%d %d\n", &width, &height); // Dimensions
	if (width != 256 || height != 256)
	{
		printf("Image size must be 256x256\n");
		exit(1);
	}

	fgets(buffer, sizeof(buffer), file); // Max intensity

	*dest = malloc(width * height * 3);

	for(int i = 0; i < width * height * 3; i++) {
		uint8_t v;
		fscanf(file, "%hhu", &v);
		(*dest)[i] = v;
	}

	fclose(file);

	*width_out = width;
	*height_out = height;
}


void writeImage(const char *filename, uint8_t *data, uint64_t width, uint64_t height)
{
	FILE *file = fopen(filename, "w");

	fprintf(file, "P3\n"); // Type
	fprintf(file, "#generated\n"); // Comment
	fprintf(file, "%d %d\n", (int) width, (int) height); // Dimensions
	fprintf(file, "255\n"); // Max intensity

	for(uint64_t i = 0; i < width * height * 3; i++)
		fprintf(file, "%d\n", data[i]);

	fclose(file);
}

int checkResult(uint8_t *result) {
	uint8_t *golden;
	uint64_t width, height;
	loadImage(golden_filename, &golden, &width, &height);

	for (uint64_t i = 0; i < width * height; i++) {
		if (result[i] != golden[i]) {
			free(golden);
			return 1;
		}
	}

	free(golden);
	return 0;
}

int main(void)
{
	printf("Loading image from '%s'\n", in_filename);
	uint8_t *in_data;
	uint64_t width, height;
	loadImage(in_filename, &in_data, &width, &height);

	uint64_t data_size = width * height * 3;
	uint8_t *out_data = malloc(data_size);
	printf("Configuring DFE with full bitstream\n");

	max_file_t *maxfile = PRImageFilter_init();
	max_engine_t *engine = max_load(maxfile, "*");

	printf("Running DFE\n");
	PRImageFilter_actions_t myaction = { data_size, in_data, out_data };
	PRImageFilter_run(engine, &myaction);

	printf("Partially reconfiguring DFE\n");
	max_reconfig_partial_bitstream(engine, "EdgeLaplaceKernel");
	max_reconfig_partial_bitstream(engine, "ThresholdKernel");

	printf("Running DFE\n");
	myaction.instream_input = out_data;
	PRImageFilter_run(engine, &myaction);

	max_unload(engine);

	printf("Saving image to '%s'\n", out_filename);
	writeImage(out_filename, out_data, width, height);

	printf("Checking result against '%s'\n", golden_filename);
	int ret = checkResult(out_data);
	if (!ret)
		printf("Result correct\n");
	else
		printf("Result incorrect\n");

	free(in_data);
	free(out_data);

	return ret;
}
