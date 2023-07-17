/**
* This program is a part of the FSCI program SMURF project.
*
* This program is meant to serve two purposes; A general purpose 
* file transfer tool using ZMQ and a network bandwidth measurment tool
* to help find and measure potential bottlenecks within networks in order
* to.
*
* This in intended to be run on a worker and or sensor waiting for a manager 
* to tell it that it needs a fresh set of data to run operations on.
*
* @author Noah Jones <noahjones7771031@gmail.com>, <nmjones@lps.umd.edu>
*
**/
#include <zmq.h>
#include <cksum.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sender.h> //ZMQ socket type (pair) defined withing sender.h


#define BANDWIDTH_INCREMENT_TEST 0
#define increment_size 10 //results in a 1Gb file being created and transfered

/**
* When BANDWIDTH_INCREMENT_TEST is set to 1 this program will create 
* files with incrementing sizes by powers of 2 to send indiviudally to a
* requester. Once a file is transfered the supplicant will report on the
* of the file and the time it took to transfer that file then exit after
* the number of iterations defined in INCREMENT_SIZE have processed.
*
* When BANDWIDTH_INCREMENT_TEST is set to 0 the program will 
* send only one file the report the files size and how long it
* took to transfer the file.
*
* CAUTION: there are no built in safe guards to check the size of the file the 
* program is creating, along with that the file are incremented by powers of 2 on the supplicant.
* The first file will be 2MB and if we set our iteration to 5 you will end up receiving 
* a 32MB file, 10 iteration will get you a 1GB file, going up to 20 iterations will
* attempt to send you a 1048 GB file!!!

* In conclusion this program could easily cause a multitude of issues including; 
* data corrupiton, file system instability, and many other general operating system 
* errors, So always ensure the increment size is set to a level at which your storage 
* and file system can handle.
*
* NOTE: I do intend to implement checksums to verify file integrity in the future.
**/

int main (void)
{
    //create ZMQ context
    void *context = zmq_ctx_new ();
    //Attemp to bind socket
    void *responder = bind_socket(context, "tcp://*:8888");
    
    //check that responder(socket) was succefully created and binded
    if (responder == NULL){
    	printf("Failed to bind to client.");
    	//clean up in event of an error
    	zmq_close(responder);
    	zmq_ctx_destroy(context);
    	return 1;}
    
    //Loop in order to allow supplicant to provide multiple files instead of exiting after a single transfer
    while(1){
    	//Attemp to recieve file path from requester
    	char path[256];
    	memset(path, 0, 256);
    	zmq_recv(responder, path, 256,0);
    
    	//Check if a path was received from a requester
    	if (strlen(path) != 0){
    		//Check if we are doing a bandwidth test or transfering a single file
    		//bandwidth test section
    		if (BANDWIDTH_INCREMENT_TEST == 1){
		    	//send file to requester
		    	const char* filename = "ITF";
		    	off_t filesize = 2 * 1024 * 1024;
		    	printf("Supplicant chuck size: %d\n", FILE_CHUNK_SIZE);
		    	int i=0;
		    	for(i; i<increment_size; i++){
		    		if (genfile(filename, filesize) != 0){
		    			//fprintf(stderr, "Failed to create file on iteration %d\n", i + 1);
		    			return 1;}
		    		filesize *=2;

		    		int result = send_file_to_requester(responder, path);
		    		if (result != 0){return -1;}}}
		//single file transfer section
		//single file transfer will include a MD5 checksum value
		if (BANDWIDTH_INCREMENT_TEST == 0){
			printf("Supplicant chuck size: %d\n", FILE_CHUNK_SIZE);
			//calculate MD5 checksum for 
			char checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
			if(calc_md5_sum(path, checksum_str)){
			printf("MD5 checksum for %s: %s\n",path, checksum_str);
			//send MD5 sum to requester before we start to send it the file
			zmq_send(responder, checksum_str, sizeof(checksum_str), 0);
			}
			
			int result = send_file_to_requester(responder, path);
			if (result != 0){return -1;}}}
	//set path to 0s to avoid potential reruns
	memset(path, 0, 256);}
	
    //clean up zmq resources
    zmq_close(responder);
    zmq_ctx_destroy(context);
    
    return 0;}






