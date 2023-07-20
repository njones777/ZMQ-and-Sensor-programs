#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <zmq.h>
#include <stdio.h>
#include <cksum.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>


#define FILE_CHUNK_SIZE 4096
#define REQUEST "request for archive"
#define APPROVE "request approved"
#define CSV_FILE_LIMIT 5
//#define PARTIAL_PATH "CSV_ARCHIVE/test"



int receive_file_from_catalogger(void *socket, int file_counter);
int csv_count();


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
	
	int file_counter = 0;
	while(1){
		int csvs = csv_count();
		if (csvs < CSV_FILE_LIMIT){
			int res = receive_file_from_catalogger(socket, file_counter);
			file_counter++;
		}
		else{printf("there are %d CSVs thats too many\n", csvs);}
	}
	/*zmq_close(socket);
	zmq_ctx_destroy(context);
	return NULL;*/
}

int receive_file_from_catalogger(void *socket, int file_counter){
	
	//Listen for request for arhchive(s) from catalogger
	char response[32];
	memset(response, 0, 32);
	zmq_recv(socket, response, 32,0);
	//int bytes_received = zmq_recv(socket, response, 32, 0);
	//check if a request for archival has been sent
	if(strcmp(response, REQUEST) != 0){return 1;}
	
	
	//send the approve for request
	zmq_send(socket, APPROVE, strlen(APPROVE), 0);
	
	//Receive file name from catalogger
	char LOCAL_PATH[128];
	//file with designation from sensor will be a single letter
	char RPbuffer[2];
	zmq_recv(socket, &RPbuffer, sizeof(RPbuffer)-1, 0);
	
	//See what we receive
	printf("Received %s\n", RPbuffer);
	//modif received file distinction with directory, iteration number and file type
	sprintf(LOCAL_PATH, "%s%s%d%s","CSV_ARCHIVE/", RPbuffer, file_counter, ".csv");
	printf("Path to write to: %s\n", LOCAL_PATH);
	
	size_t sb = strlen(LOCAL_PATH) + 1;
	printf("size of local path is: %zu bytes\n", sb);
	
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
	if (calc_md5_sum(LOCAL_PATH, local_checksum_str)){
	if(strcmp(received_checksum_str, local_checksum_str) == 0){
    		printf("Checksums validation complete\n");}
    	else{
    		printf("File corrupted, Take appropriate Action\nLocal checksum: %s\n",local_checksum_str); 
    		return 1;} 
	}
	
return 0;}


int csv_count(){
// Replace this with the path to your directory
    const char* directoryPath = "/home/nmjones/Desktop/ZMQ_git/ZMQ-and-Sensor-programs/archiver/CSV_ARCHIVE"; 
    int csvFileCount = 0;

    DIR* directory = opendir(directoryPath);
    if (directory == NULL) {
        perror("Error opening directory");
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_REG) { // Check if it's a regular file
            const char* fileName = entry->d_name;
            size_t fileNameLength = strlen(fileName);

            // Check if the file ends with ".csv"
            if (fileNameLength > 4 && strcmp(&fileName[fileNameLength - 4], ".csv") == 0) {
                csvFileCount++;}}}

    closedir(directory);

    return csvFileCount;}


#endif













