#ifndef CATALOGGER_H
#define CATALOGGER_H

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cksum.h>
#include <pthread.h>
//#include <openssl/md5.h>

#define FILE_CHUNK_SIZE 4096
#define BUFFER_SIZE 256
#define REQUEST "request for archive"
#define APPROVE "request approved"


//Function that handles socket creation and binding to help make main code cleaner
void* connect_to_archivist(void *context, const char *server_address){
	//create socket and attempt to connect to provided context
	void *socket = zmq_socket(context, ZMQ_PAIR);
	if (socket == NULL){
		printf("CTA_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
	}
	//attempt to connect socket to provided server
	int rc = zmq_connect(socket, server_address);
	if (rc != 0){
		printf("CTA_error 2: Failed to Connect to server. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);
	}
	printf("Connection succesful to archivist at %s\n", server_address);
	
	//sanity check
	//char message[]="hello";
	//zmq_send(socket, message, strlen(message), 0);
	return socket;}
	
	
	
//function to receive requested file and attempt to send requested file back
int send_file_to_archivist(void *socket, char *path){

	//Check if sensor has or can even open file before initiating request
	FILE* file = fopen(path, "rb");
	if (file == NULL){
		printf("Unable to open %s\n", path);
		return -1;
		}

	printf("sending %s to archivist\n", REQUEST);
	//send message asking if archivist is alive and ready to recieve an archive file
   	zmq_send(socket, REQUEST, strlen(REQUEST), 0);
    
    	//listen for response from archivist
	char response[32];
	memset(response, 0, 32);
	zmq_recv(socket, response, 32, 0);
	//printf("got %s from archivist\n", response);
	//check if a request for archival has been sent
	if(strcmp(response, APPROVE) != 0){return -1;}
    
    	//calculate and send MD5 checksum to archivist for file integrity validation
    	char checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
    	if(calc_md5_sum(path, checksum_str)){
	printf("MD5 checksum for %s: %s\n",path, checksum_str);
	//send MD5 sum to requester before we start to send it the file
	zmq_send(socket, checksum_str, sizeof(checksum_str), 0);}
	
	//////////////////////////////
	//Begin actual file transfer//
	//////////////////////////////
	
	
	//FILE* file = fopen(path, "rb");
	//move the file position indicator to the end of the file specified by the FILE pointer file
	fseek(file, 0, SEEK_END);
	//determine the current position of the file position indicator
	long file_size = ftell(file);
	//reset file position indicatior to the beginning of the file 
	rewind(file);
	
	//send the file size to the receiver
	zmq_send(socket, &file_size, sizeof(long), 0);
	
	//send the file content in chunks
	char buffer[FILE_CHUNK_SIZE];
	size_t remaining_bytes = file_size;
	while (remaining_bytes > 0) {
		
		size_t chunk_size = fread(buffer, sizeof(char), FILE_CHUNK_SIZE, file);
		zmq_send(socket, buffer, chunk_size, 0);
		remaining_bytes -= chunk_size;
	}
	//Cleanup
	fclose(file);
	return 0;}











#endif
