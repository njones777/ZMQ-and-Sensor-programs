CC = cc

all: convertFM

convertFM: convertFM.o fft_file_handling.o fft_arrays.o
	$(CC) -o convertFM convertFM.c fft_file_handling.c fft_arrays.c -lfftw3 -lm

fft_file_handling.o: fft_file_handling.c fft_file_handling.h
	$(CC) -c fft_file_handling.c

fft_arrays.o: fft_arrays.c fft_arrays.h
	$(CC) -c fft_arrays.c -lfftw3

convertFM.o: convertFM.c
	$(CC) -c convertFM.c

clean:
	rm *.o convertFM
