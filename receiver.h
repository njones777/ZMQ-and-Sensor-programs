/**
* This program is a part of the FSCI program SMURF project 
*
* receiver.h - provides additonal functionaly and helper functions for 'request.c'
* file. Includes various libraries, defines constants, funcitons to aid in file operations
* and socket handling.
*
*@author Noah Jones <noahjones7771031@gmail.com>, <nmjones@lps.umd.edu>

**/

#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define FILE_CHUNK_SIZE 1048576 //1MB

//Variables for program performance (execution time of file receiving operation)
    struct timeval start_time, end_time;
    double execution_time;


//Assume user has already created context in their main file as the overhead is not worth it
void* connect_to_supplicant(void *context, const char *server_address){
	//create socket and attempt to connect to provided context
	void *socket = zmq_socket(context, ZMQ_PAIR);
	if (socket == NULL){
		printf("CTS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
	}	
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(socket, server_address);
	if (rc != 0){
		printf("CTS_error 2: Failed to Connect to server. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);
	}
	printf("Connection succesful to server at %s\n", server_address);
	return socket;}
	
//Gets resulting file size in human readable format
char* getFileSizeH(long size){
    char* result = (char*)malloc(20 * sizeof(char)); // Allocate memory for the result string
    if (size < 1024) {
        sprintf(result, "%ld bytes", size);
    } else if (size < 1024 * 1024) {
        sprintf(result, "%.2f KB", (double)size / 1024);
    } else if (size < 1024 * 1024 * 1024) {
        sprintf(result, "%.2f MB", (double)size / (1024 * 1024));
    } else {
        sprintf(result, "%.2f GB", (double)size / (1024 * 1024 * 1024));
    }

    return result;}


int receive_file_from_device(void *socket,const char *local_path){
	
	//Check for repsonse from supplicant on status of file
	//I.E. wheter it can be opened or not, or if it even exists
	char response[256];
	memset(response, 0, 256);
	zmq_recv(socket, response, 255, 0);
	if(strstr(response, "Error") != NULL){
	printf("%s\n", response);
	return -1;}
	//If the file does exist and is able to be opened the server will send back "continue"
	else if(strcmp(response, "Continue") == 0){}



	//RECORD START TIME OF FILE TRANSFER
    	gettimeofday(&start_time, NULL);
	
	//receive the file size from the client
	long file_size;
	zmq_recv(socket, &file_size, sizeof(long), 0);
	//recieve the file content in chunks
	char buffer[FILE_CHUNK_SIZE];
	size_t remaining_bytes = file_size;
	FILE* file = fopen(local_path, "wb");
	while (remaining_bytes > 0){
		size_t chunk_size = zmq_recv(socket, buffer, FILE_CHUNK_SIZE, 0);
		fwrite(buffer, sizeof(char), chunk_size, file);
		remaining_bytes -= chunk_size;}
	
	//cleanup
	fclose(file);
	
	//RECORD END TIME OF PROGRAM
    	gettimeofday(&end_time, NULL);
    	execution_time = (double)(end_time.tv_sec - start_time.tv_sec) +
                     (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
                     
    	//PRINT FILE SIZE AND EXECUTION TIME        
    	printf("File of size %s, received from supplicant in  %.2f seconds\n", getFileSizeH(file_size), execution_time);
	return 0;}
#endif 
