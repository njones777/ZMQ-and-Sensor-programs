/**
* This program is a part of the FSCI program SMURF project.
*
* This program is meant to server two purposes; A general purpose 
* file receiver using ZMQ and a network bandwidth measurment tool
* to help find and measure potential bottlenecks within networks.
*
* This in intended to be run on a manager when it needs data from workers
* and or sensors. 
*
* @author Noah Jones <noahjones7771031@gmail.com>, <nmjones@lps.umd.edu>
*
**/
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <receiver.h> //ZMQ socket type defined within receiver.h

#define BANDWIDTH_INCREMENT_TEST 0
#define increment_size 10 //expects a 1GB to be received

/** 
* When BANDWIDTH_INCREMENT_TEST is set to 1 this program expects to 
* receive incrementing file sizesfrom the supplicant.c program 
* and report on the size of the file and the time it took to 
* receive and write those file to disk then exit after the number
* defined in INCREMENT_SIZE have processed.
*
* When BANDWIDTH_INCREMENT_TEST is set to 0 the program will 
* recieve and write only one file to disk then exit.
*
* CAUTION: there are no built in safe guards to check the size of the file the 
* program is receiving, along with that the file are incremented by powers of 2 on the supplicant.
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

int main (int argc, char** argv)
{
    //check that the IP address, remote file path, and local file path were all provided 
    if (argc != 4){printf("Error: invalid arguments\nSyntax: ./receiver.c <SUPPLICANT_IP_ADDRESS> </Requested/file/path> <Destiantion/file/path\n");}
    int port = 8888;
    char server_addr[255];
    strcpy(server_addr, argv[1]);
    sprintf(server_addr, "tcp://%s:%d", argv[1], port);
    
    //path of file on remote device
    char *remote_path=argv[2];
    //path of file we will write to locally
    char *local_path=argv[3];

    //create zmq_context and bind socket to context
    void* context=zmq_ctx_new();
    
    //make socket and attempt to connect to listening supplicant
    void* socket = connect_to_supplicant(context, server_addr);
    //check if socket returned NULL
    if (socket == NULL){
    	printf("ERROR 1 Failed to connect to server.");
    	//clean up socket in event of error
    	zmq_close(socket);
    	zmq_ctx_destroy(context);}
    
    //send file path off to supplicant to check for file status
    zmq_send(socket, remote_path, strlen(remote_path), 0);
    
    //check if we are doing a multi file bandwidth test
    if (BANDWIDTH_INCREMENT_TEST == 1){
	int i=0;
	printf("Receiver chunk size: %d\n", FILE_CHUNK_SIZE);
	for(i; i<increment_size; i++){
		//Attempt to receive file from supplicant
		int result = receive_file_from_device(socket, local_path);
		//check if file was sucessfully received
		if (result != 0){return -1;}}}
    
    //otherwise check if we are requesting a single file from the supplicant
    else if(BANDWIDTH_INCREMENT_TEST == 0){
    	int result = receive_file_from_device(socket, local_path);
    	//check if file was successfully received
    	if (result!=0){return -1;}}
    
    //Cleanup after receiving the file
    zmq_close(socket);
    zmq_ctx_destroy(context);
    
    return 0;}



