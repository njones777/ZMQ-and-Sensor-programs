# Luke Wilson
# 
# To use:
# rtl_fm -M wbfm -r 48000 -f 102.7M output.bin
# python3 convertFM.py 102.7 2000
#
# Note: First command-line argument is the station frequency (i.e. 91.5)
# Note: Second command-line argument is the maximum FFT frequency (i.e. 2000)
#
# File will...
# 1. Look for a binary file "output.bin" of bytes straight from rtl_fm receiver 
# 2. Read those bytes into a numpy array
# 3. Trim off tail if desired
# 4. Plot signal if desired
# 5. Perform an FFT on the data
# 6. Plot FFT if desired
# 7. Save audio if desired (.wav)
# 8. Save CSV if desired [station, frequency, norm]

import scipy
import scipy.fftpack
import scipy.io.wavfile
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

# ----------------
# SET PARAMETERS
station = sys.argv[1]
max_frequency = int(sys.argv[2])
input_file_name = "output.bin"
sample_points = 1000
full_sample_points = True # Use ALL sample points in file?
collection_rate = 1008000
sample_rate = 48000
plot_signal = False
plot_fft = False
save_sound = False
zero_zero = False # Set norm at frequency zero to zero in FFT? (Makes it more readable)
save_csv = True
# ----------------

# Import data
data = np.fromfile(input_file_name, dtype = np.int16)

# Trim data
if full_sample_points:
    sample_points = len(data)
else:
    data = data[:sample_points]

# Plot data
if plot_signal:
    plt.plot(data)
    plt.xlabel("Time")
    plt.ylabel("Demodulated Signal")
    plt.title("FM Signal")
    plt.savefig("signal_plot_FM.pdf")
    plt.clf()

# Set FFT Parameter
sample_spacing = 1.0 / collection_rate

# Run FFT
fft_data = scipy.fft.fft(data)
fft_freq = scipy.fftpack.fftfreq(sample_points, sample_spacing)[0:sample_points//2]

# Trim high frequencies
max_fft_sample = int(np.floor(max_frequency * sample_points / collection_rate)) + 1
fft_data = fft_data[:max_fft_sample]
fft_freq = fft_freq[:max_fft_sample]

# Trim off 0
if zero_zero:
    fft_data[0] = 0.0 + 0.0j


# Plot FFT
if plot_fft:
    plt.plot(fft_freq, 2.0 / sample_points * np.abs(fft_data[0:sample_points//2]))
    plt.xlim(0, max_frequency)
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Strength")
    plt.title("FFT of FM Signal")
    plt.grid()
    plt.savefig("fft_plot_FM.pdf")

# Write data to file
if save_sound:
    scaled_data = np.int16(data / np.max(np.abs(data)) * 32000)
    scipy.io.wavfile.write("output_audio_FM.wav", sample_rate, scaled_data)

# Write data to csv
if save_csv:
    matrix_df = pd.DataFrame(list(zip(np.repeat(station, sample_points/2), fft_freq, 2.0 / sample_points * np.abs(fft_data[0:sample_points//2]))))
    matrix_df.columns = ["station", "frequency", "norm"]
    matrix_df.to_csv("fft_radio.csv", index=False)


