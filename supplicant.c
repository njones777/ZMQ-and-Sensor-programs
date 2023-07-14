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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sender.h>

#define BANDWIDTH_INCREMENT_TEST 0
#define increment_size 10

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
		if (BANDWIDTH_INCREMENT_TEST == 0){
			printf("Supplicant chuck size: %d\n", FILE_CHUNK_SIZE);
			int result = send_file_to_requester(responder, path);
			if (result != 0){return -1;}}}
	//set path to 0s to avoid potential reruns
	memset(path, 0, 256);}
	
    //clean up resources
    zmq_close(responder);
    zmq_ctx_destroy(context);
    
    return 0;}






