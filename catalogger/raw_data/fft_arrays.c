/* Luke Wilson
 * File for handling array operations independent of file operations
 * SMURF project with Tyler Simon
 * C file
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fftw3.h>

// Function to convert array of int16s to array of doubles
double * int16_to_double(int16_t * old_array, int array_length){

	// Allocate memory
	double * new_array = (double*)malloc(array_length * sizeof(double));
	if (new_array == NULL){
		printf("Failed to convert int16s to doubles :(\n");
		exit(1);
	}

	// Iterate and convert data
	int i = 0;
	for (i = 0; i < array_length; i++){
		new_array[i] = (double) old_array[i];
	}

	return new_array;
}

// Function to print out head of int16 array
void head_int16(int16_t * array, int n){
	int i = 0;
	for (i = 0; i < n; i++){
		printf("%d\n", array[i]);
	}
}

// Function to print out head of double array
void head_double(double * array, int n){
        int i = 0;
        for (i = 0; i < n; i++){
                printf("%lf\n", array[i]);
        }
}

// Function to take norm of array of fftw_complex values (requires double array to be initialized with memory allocated)
void fftw_complex_to_norm(fftw_complex * complex_array, double * double_array, int length){
	int i = 0;
	for (i = 0; i < length; i++){
		double real = complex_array[i][0];
		double imag = complex_array[i][1];
		double_array[i] = sqrt(real * real + imag * imag);
	}
}
