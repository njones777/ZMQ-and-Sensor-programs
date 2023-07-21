/* Luke Wilson
 * File for handling array operations independent of file operations
 * SMURF project with Tyler Simon
 */

#ifndef FFT_ARRAYS_H
#define FFT_ARRAYS_H

#include <stdlib.h>
#include <stdio.h>

// Function to convert array of int16s to array of doubles
double * int16_to_double(int16_t * old_array, int array_length);
// Function to print out head of int16 array
void head_int16(int16_t * array, int n);
// Function to print out head of double array
void head_double(double * array, int n);
// Function to take norm of array of fftw_complex values (requires double array to be initialized with memory allocated)
void fftw_complex_to_norm(fftw_complex * complex_array, double * double_array, int length);
#endif
