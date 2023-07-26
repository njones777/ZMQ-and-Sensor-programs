#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <zmq.h>
#include <file.h>
#include <stdio.h>
#include <cksum.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <socket.h>
#include <zmq_helpers.h>

#define MAXLEN 512
#define FILE_CHUNK_SIZE 4096
//port to connect to manager
#define archiver_to_manager_port 8888
#define REQUEST "request for archive"
#define APPROVE "request approved"
//address of manager
//#define MANAGER_ADDR "10.10.40.227"
#define MANAGER_ADDR "169.254.2.39"
//archivist wireless ip address to connect to manager
//#define ARCHIVER_WIRELESS_IP "10.10.40.207"
#define ARCHIVER_WIRELESS_IP "169.254.2.52"
//where the merged CSV file will go
#define CSV_LOCAL_PATH "CSV_STAGED/merged.csv"

//struct containining paramaters to send to sensors
struct RPI_params{
	const char* port;
	float frequency;
	int batch;
};

int receive_file_from_catalogger(void *socket, int file_counter);



// function for thread(s) to handle socket
void *thread_gather_archives(void* arg){
	
	//receive struct
	struct RPI_params* params = (struct RPI_params*)arg;
	
	void *context=zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_PAIR);
	
	int rc = zmq_bind(socket, params->port);
	
	if (rc !=0){
		perror("Error binding socket");
		return NULL;
		}
		
	//send paramters to pi
	//send frequency
	char fbuffer[20];
	char bbuffer[10];
	snprintf(fbuffer, sizeof(fbuffer), "%f", params->frequency);
	snprintf(bbuffer, sizeof(bbuffer), "%d", params->batch);
	zmq_send(socket, fbuffer, sizeof(fbuffer), 0);
	//send batch size
	zmq_send(socket, bbuffer, sizeof(bbuffer), 0); 
	
	int csvs=0;
	while(1){
		int res = receive_file_from_catalogger(socket, csvs);
		if (res == 0){csvs++;}
		if (csvs==params->batch){break;}
	}
	zmq_close(socket);
	zmq_ctx_destroy(context);
}

void* connect_to_manager(){
	char server_addr[MAXLEN];
	char host[55];
	gethostname(host, sizeof(host));
	host[MAXLEN - 1] = '\0';
	sprintf(server_addr, "tcp://%s:%d", MANAGER_ADDR, archiver_to_manager_port);
	printf("(client, manager) = (%s, %s)\n", host, server_addr);
	void *context=zmq_ctx_new();
	void* public = connect_to_supplicant(context, server_addr);

	// Check if socket returned NULL
	if (public == NULL){
	    printf("ERROR 1 Failed to connect to server.");
	    zmq_close(public);
	    zmq_ctx_destroy(context);
	}
	// Attempt to bind private socket
		void *client = bind_socket(context, "tcp://*:8888");
		char sendbuffer[MAXLEN];
		sprintf(sendbuffer, "client;checkin;%s;%s",ARCHIVER_WIRELESS_IP, "archiver");
		zmq_send(public, sendbuffer, sizeof(sendbuffer), 0);
		sleep(1);
		sprintf(sendbuffer, "client;request");
		zmq_send(public, sendbuffer, sizeof(sendbuffer), 0);
		zmq_close(public);
	return client;
}

void get_params_from_manager(void *socket, char** freq, char** batch){
	char params[64];
	zmq_recv(socket, params, sizeof(params), 0);
	int numtokens;
	char** split_params=splitStringOnSemiColons(params, &numtokens);
	*freq=split_params[0];
	*batch=split_params[1];
	}
	
	

void *extract_archive(void* arg) {
	void* socket = arg;
	system("python3 mergeMore.py");
	//printf("CSV LIMIT REACHED SENDING MERGED CSV");
	system("rm CSV_ARCHIVE/*");
	 	
	char path[256];
	char sendbuffer[MAXLEN];
	memset(path, 0, 256);

	// Set file path
	strcpy(path, CSV_LOCAL_PATH);

	if (strlen(path) != 0){
		//calculate MD5 checksum for 
		char checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
		if(calc_md5_sum(path, checksum_str)){
		printf("MD5 checksum for %s: %s\n",path, checksum_str);

		// Send MD5 sum to manager before we start to send it the file
		zmq_send(socket, checksum_str, sizeof(checksum_str), 0);
				}    
				
		// Begins to send file to manager
		int result = send_file_to_requester(socket, path);
		if (result != 0){printf("FILE NOT SENT");}}
		    
	//set path to 0s to avoid potential reruns
	memset(path, 0, 256);
	return 0;}

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
	
	//modif received file distinction with directory, iteration number and file type
	sprintf(LOCAL_PATH, "%s%s%d%s","CSV_ARCHIVE/", RPbuffer, file_counter, ".csv");
	//printf("Path to write to: %s\n", LOCAL_PATH);
	
	//get size in bytes of local path to write received CSV to
	/*size_t sb = strlen(LOCAL_PATH) + 1;
	printf("size of local path is: %zu bytes\n", sb);*/
	
	//receive MD5 checksum from catalogger
    	char received_checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
    	char local_checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
    	
    	zmq_recv(socket, &received_checksum_str, sizeof(received_checksum_str), 0);
	printf("MD5 checksum: %s\n", received_checksum_str);
	
	
	//Receive file size from a catalogger
	int file_size;
	zmq_recv(socket, &file_size, sizeof(int), 0);
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

#endif













