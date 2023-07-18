#!/bin/bash


file="output.wav"

#begin collecting on command line specified channel
( rtl_fm -M wbfm -f $1M | sox -t raw -e signed-integer -b 16 -c 1 -r 44100 - $file ) > /dev/null 2>&1 &
echo rtl_fm started

#infinite loop to check size of sensor data file
while true
do

	#check if file exits and if it is greater than 500KB
	if [ -s $file ] &&  [ $(stat -c %s $file) -gt 512000 ]; then 
		# use pgrep to get the process IDs of the running processes with the given filename
		pids=$(pgrep -f "rtl_fm")
		for pid in $pids; do
			#ps aux | awk -v pid="$pid" '$2 == pid { print }'
			kill -9 "$pid"
			echo process killed
		done
		python3 wavs/batch_fft.py output.wav 2048 0
		mv fft_audio.csv CSVs/
		rm $file
		./records/catalog
		
		break
	fi
		#after the file has been written to the specific size we now run FFT on it
done

