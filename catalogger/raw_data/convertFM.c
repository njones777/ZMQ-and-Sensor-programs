/* Luke Wilson
 * Substitute for convertFM.py
 * Computes the FFT with fftw3 package
 * Input file: output.bin
 * Output file: fft_radio.csv
 * Output format: [station, frequency, norm]
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fftw3.h>
#include "fft_file_handling.h"
#include "fft_arrays.h"

int main(int argc, char ** argv){
	
	// Confirm proper use
	if (argc != 3){
		printf("Improper use :(\n");
		printf("./fft_rec <station (xx.x)> <max frequency to keep (int)>\n");
	}
	
	// Pull station number
	const char * station = argv[1];
	
	// Pull max frequency to keep
	const int max_frequency = atoi(argv[2]);

	// ---------------
	// SET PARAMETERS
	//const int sample_points = 1000;
	//const int full_sample_points = 1; // boolean
	const int collection_rate = 1008000;
	const int sample_rate = 48000;
	const int zero_zero = 0; // boolean
	// ---------------
	
	// Open file
	FILE * input_file = fopen("output.bin", "rb");
	if (input_file == NULL){
		printf("Error opening input file! :(\n");
		return 1;
	}	
	
	// Define file size
	int data_length;
	int * data_length_ptr = &data_length;
	
	// Read in .bin data
	int16_t * raw_data = binfile_to_int16_array(input_file, data_length_ptr);
	
	// Convert to double
	double * double_data = int16_to_double(raw_data, data_length);
	
	// Define FFT output
	fftw_complex * fft_output = fftw_alloc_complex(data_length / 2 + 1);
	if (fft_output == NULL){
		printf("Failed to allocate memory for fft_output :(\n");
		free(raw_data);
		free(double_data);
		fclose(input_file);
		exit(1);
	}

	// Make a plan
	fftw_plan the_plan = fftw_plan_dft_r2c_1d(data_length, double_data, fft_output, FFTW_ESTIMATE);
	
	// Execute the plan
	fftw_execute(the_plan);
	
	// Calculate length of FFT
	const int fft_length = data_length / 2 + 1;
	
	// Calculate index of maximum frequency
	const int max_fft_index = max_frequency * data_length / collection_rate + 1;

	// Take norm of FFT
	double * fft_output_norm = (double*) malloc(max_fft_index * sizeof(double));
	fftw_complex_to_norm(fft_output, fft_output_norm, max_fft_index);

	// Rescale norm of FFT
	int i = 0;
	for (i = 0; i < max_fft_index; i++){
		fft_output_norm[i] = 2.0 / (double) data_length * fft_output_norm[i];
	}

	// Open output CSV
	FILE * output_file = fopen("fft_radio.csv", "w");
	if (output_file == NULL){
		printf("Failed to open fft_radio.csv :(\n");
		free(raw_data);
		free(double_data);
		exit(1);
	}

	// Calculate fundamental frequency
	double fundamental_freq	= (double) collection_rate / (double) data_length;

	// Write first line to CSV
	fprintf(output_file, "station,frequency,norm\n");

	// Iterate and write to CSV
	double freq;
	i = 0;
	for (i = 0; i < max_fft_index; i++){
		freq = (double) i * fundamental_freq;
		fprintf(output_file, "%s,%f,%f\n", station, freq, fft_output_norm[i]);
	}
	

	// Close file, free arrays
	fclose(output_file);
	free(raw_data);
	free(double_data);

	return 0;
}
