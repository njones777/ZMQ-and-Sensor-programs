/* Luke Wilson
 * fft_file_handling.h
 * For reading from and writing to files for the FFT folder
 * SMURF project with Tyler Simon
 */

#ifndef FFT_FILE_HANDLING_H
#define FFT_FILE_HANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read in from .bin file
int16_t * binfile_to_int16_array(FILE * file, int * array_length_ptr);

#endif
