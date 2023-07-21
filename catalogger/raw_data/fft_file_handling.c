/* Luke Wilson
 * fft_file_handling.c
 * For reading from and writing to files for the FFT folder
 * SMURF project with Tyler Simon
 * C file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read in from .bin file
int16_t * binfile_to_int16_array(FILE * file, int * array_length_ptr){
	
	// Determine file size
	fseek(file, 0, SEEK_END);
	const int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Determine number of values
	*(array_length_ptr) = file_size / sizeof(int16_t);

	// Allocte memory for array
	int16_t * data = (int16_t*) malloc(*(array_length_ptr) * sizeof(int16_t));
	if (data == NULL){
		printf("Couldn't read file :(\n");
		fclose(file);
		exit(1);
	}

	// Read file contents
	fread(data, sizeof(int16_t), *(array_length_ptr), file);

	// Close file
	fclose(file);

	return data;

}
