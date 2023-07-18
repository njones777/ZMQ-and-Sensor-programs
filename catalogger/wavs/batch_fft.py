# Luke Wilson
# Program to be used with collect_audio.c
# Usage:
# python3 batch_fft.py temp.wav <batch size>
# What it does:
# Performs an FFT on the temp.wav file
# Then constructs a matrix with five columns
# Writes that matrix to fft_audio.csv
# QUICK USAGE:
# python3 batch_fft.py <file_name.wav> 65536
# :)

import numpy as np
import scipy
from scipy.io.wavfile import read, write
import scipy.fftpack
import pandas as pd
import sys

# -----------------
# SET PARAMETERS
fft_data_type = float
file_data_type = np.int16
print_data = False # print (very truncated) version of raw data?
reduced = True # 2 columns only (frequency, norm)
max_frequency = 20000 # cut off above this point
sample_rate = 48000
# -----------------

# Initialize batch number
file_name = sys.argv[1]
batch_size = int(sys.argv[2])
        
# Read in .wav file
audio = read(file_name)

# Convert to numpy
data = np.array(audio[1], dtype=fft_data_type)[:batch_size]

# Print
if print_data:
    print("Raw data:")
    print(data)

# Set FFT Parameters
sample_spacing = 1.0 / sample_rate
sample_points = len(data)

# Run FFT
fft_data = scipy.fft.fft(data)
fft_freq = scipy.fftpack.fftfreq(sample_points, sample_spacing)[0:sample_points//2]
fft_data_abs = 2.0 / sample_points * np.abs(fft_data[0:sample_points//2])
if print_data:
    print("FFT Frequencies:")
    print(fft_freq)
    print("FFT:")
    print(fft_data_abs)

# Trim FFT
max_fft_sample = int(np.floor(max_frequency * batch_size / sample_rate))
fft_data_abs = fft_data_abs[:max_fft_sample]
fft_freq = fft_freq[:max_fft_sample]
        
# Turn to Pandas DF
if reduced:
    matrix_df = pd.DataFrame(list(zip(fft_freq, fft_data_abs)))
    matrix_df.columns = ["frequency", "norm"]
else:
    matrix_df = pd.DataFrame(list(zip(np.repeat("detect", batch_size/2), fft_freq, fft_data_abs, np.repeat(batch_number, batch_size/2), np.repeat(0, batch_size/2))))
    matrix_df.columns = ["name", "frequency", "norm", "batch", "receiver"]

# Export matrix
matrix_df.to_csv("fft_audio.csv", index=False)
