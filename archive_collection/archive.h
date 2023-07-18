#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <zmq.h>
#include <stdio.h>
#include <cksum.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <pthread.h>

#define FILE_CHUNK_SIZE 4096
#define REQUEST "request for archive"
#define APPROVE "request approved"
#define LOCAL_PATH "/home/nmjones/Desktop/ZMQ_git/ZMQ-and-Sensor-programs/archive_collection/test.csv"



int receive_file_from_catalogger(void *socket);


// function for thread(s) to handle socket
void *thread_gather_archives(void *arg){
	char *port = (char *)arg;
	
	
	void *context=zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_PAIR);
	
	int rc = zmq_bind(socket, port);
	
	if (rc !=0){
		perror("Error binding socket");
		return NULL;
		}
	
	while(1){
		/*char response[32];
		memset(response, 0, 32);
		zmq_recv(socket, response, 32, 0);
		printf("%s\n", response);*/
		int res = receive_file_from_catalogger(socket);	
		//printf("Result is %d\n", res);
	}
	/*zmq_close(socket);
	zmq_ctx_destroy(context);
	return NULL;*/
}

int receive_file_from_catalogger(void *socket){
	
	//Listen for request for arhchive(s) from catalogger
	char response[32];
	memset(response, 0, 32);
	zmq_recv(socket, response, 32,0);
	//int bytes_received = zmq_recv(socket, response, 32, 0);
	//check if a request for archival has been sent
	if(strcmp(response, REQUEST) != 0){return 1;}
	
	
	//send the approve for request
	zmq_send(socket, APPROVE, strlen(APPROVE), 0);
	
	
	//receive MD5 checksum from catalogger
    	char received_checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
    	char local_checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
    	
    	zmq_recv(socket, &received_checksum_str, sizeof(received_checksum_str), 0);
	printf("MD5 checksum: %s\n", received_checksum_str);
	
	
	//Receive file size from a catalogger
	long file_size;
	zmq_recv(socket, &file_size, sizeof(long), 0);
	
	//recieve the file content in chunks
	char buffer[FILE_CHUNK_SIZE];
	size_t remaining_bytes = file_size;
	
	//open file in write binary mode
	FILE* file = fopen(LOCAL_PATH, "wb");
	while (remaining_bytes > 0){
		size_t chunk_size = zmq_recv(socket, buffer, FILE_CHUNK_SIZE, 0);
		fwrite(buffer, sizeof(char), chunk_size, file);
		remaining_bytes -= chunk_size;}
	//cleanup
	fclose(file);
	
	//Calculate MD5 checksum for received file to verify file integrity
	/*if (calc_md5_sum(LOCAL_PATH, local_checksum_str)){
	if(strcmp(received_checksum_str, local_checksum_str) == 0){
    		printf("Checksums validation complete\n");}
    	else{
    		printf("File corrupted, Take appropriate Action\nLocal checksum: %s\n",local_checksum_str); 
    		return 1;} 
	
	
	}*/
	
return 0;}
#endif













