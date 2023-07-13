//program that handles socket binding and sending file to requester
//Author: Noah J.
#include <zmq.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_CHUNK_SIZE 1048576
#define BUFFER_SIZE 256


//Variables for program performance
    struct timeval start_time, end_time;
    double execution_time;

int genfile(const char* filename, off_t size){
    int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Failed to create file");
        return 1;
    }

    if (ftruncate(fd, size) != 0) {
        perror("Failed to set file size");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;}
    
    
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



//Function that handles socket creation and binding to help make main code cleaner
void* bind_socket(void *context, const char *server_address){
	void *socket = zmq_socket(context, ZMQ_PAIR);
	//check if socket was successfully created
	if (socket == NULL){
		printf("BS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;}
	
	//attempt to bind socket
	int rc = zmq_bind(socket, server_address);
	//check status of bind operation
	if (rc != 0){
		printf("BS_error 2: Failed to bind socket. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);}
	
	return socket;
}


//function to receive requested file and attempt to send requested file back
int send_file_to_requester(void *socket, char *path){

	
	//receive file path from requester
	//char path[BUFFER_SIZE];
	//memset(path, 0, BUFFER_SIZE);
	//zmq_recv(socket, path, BUFFER_SIZE, 0);
	
	
	//attempt to open file
	FILE* file = fopen(path, "rb");
	
	//send status back to requester regarding if the file existed or could be opened
	if (file == NULL){
		char error_message[BUFFER_SIZE];
		snprintf(error_message, sizeof(error_message), "Error opening file: %s", strerror(errno));
		zmq_send(socket, error_message, strlen(error_message), 0);
		return -1;
	}
	//if the file exists and is able to be opened then send back the string continue
	//there is probably a more efficent way to do this but I was just trying to handle errors here
	else{
		char response[]="Continue";
		zmq_send(socket, response, strlen(response), 0);
	
	}
	//////////////////////////////
	//Begin actual file transfer//
	//////////////////////////////
	
	
	//RECORD START TIME OF FILE TRANSFER
    	gettimeofday(&start_time, NULL);
	
	//move the file position indicator to the endof the file specified by the FILE pointer file
	fseek(file, 0, SEEK_END);
	//determine the current position of the file position indicator
	long file_size = ftell(file);
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
	
	//RECORD END TIME OF PROGRAM
    	gettimeofday(&end_time, NULL);
    	execution_time = (double)(end_time.tv_sec - start_time.tv_sec) +
                     (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
                     
    	//PRINT FILE SIZE AND EXECUTION TIME            
    	printf("File of size %s, transfered to requester in  %.2f seconds\n", getFileSizeH(file_size), execution_time);
	return 0;
}














