#!/bin/bash


#file our rtl_fm command will output to
file="output.bin"
frequency=$1
max_frequency=$2
rf="${frequency}m"

#check for running rtl_fm proccesses and kill them before starting this rtl_fm command
#this helps prevent multiple rtl_fm proccesses from being run which would render the 
#rtl_sdr antenna unsuable
pids=$(pgrep -f "rtl_fm")
for pid in $pids; do
	kill -9 $pid
done

#begin collecting on command line specified channel
( rtl_fm -M wbfm -r 48000 -f $rf $file ) > /dev/null 2>&1 &
while true
do
	#check if file exits and if it is greater than 500KB
	if [ -s $file ] &&  [ $(stat -c %s $file) -gt 512000 ]; then 
		# use pgrep to get the process IDs of the running processes with the given filename
		pids=$(pgrep -f "rtl_fm")
		for pid in $pids; do
			#kill rtl_fm process
			kill -15 $pid
		done
		#converted output.bin file to a csv using fft
		../raw_data/convertFM $frequency $max_frequency
		#move generated CSV to CSV directory to be sent off
		mv fft_radio.csv ../CSVs/
		#delete ouput.bin
		rm $file
		#send generated CSV file to archivist
		#./records/catalog
		break
	fi
done
		
