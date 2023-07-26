#!/bin/bash


#file our rtl_fm command will output to
file="output.bin"
frequency=$1
rf="${frequency}m"

#infinite loop to run rtl_fm with different frequencys

#begin collecting on command line specified channel
( rtl_fm -M wbfm -r 48000 -f $rf $file ) > /dev/null 2>&1 &
#rtl_fm -M wbfm -r 48000 -f $rf $file &
#echo rtl_fm started
while true
do
	#check if file exits and if it is greater than 500KB
	if [ -s $file ] &&  [ $(stat -c %s $file) -gt 512000 ]; then 
		# use pgrep to get the process IDs of the running processes with the given filename
		pids=$(pgrep -f "rtl_fm")
		for pid in $pids; do
			#ps aux | awk -v pid="$pid" '$2 == pid { print }'
			#kill rtl_fm process
			#kill -INT "$pid" #> /dev/null 2>&1
			kill -9 $pid
		done
		#converted output.bin file to a csv using fft
		../raw_data/convertFM $frequency 2000
		#move generated CSV to CSV directory to be sent off
		mv fft_radio.csv ../CSVs/
		#delete ouput.bin
		rm $file
		#send generated CSV file to archivist
		#./records/catalog
		break
	fi
done
		
