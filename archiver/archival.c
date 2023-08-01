#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <archive.h>
#include <sys/time.h>

#define RPI1_port "tcp://169.254.2.52:8886"
#define RPI2_port "tcp://169.254.2.52:8887"

//Variables for program performance (execution time of file receiving operation)
struct timeval start_time, end_time;
double execution_time;

int main(){
	struct RPI_params params1, params2;
	params1.port=(char *)RPI1_port; params2.port=(char *)RPI2_port;
	
	//connect to manager
	void *manager_socket=connect_to_manager();
	
	//empty variables for frequency, batch, and max frequency in structs
	char *frequency1;
	char *frequency2;
	char *batch;
	char *max_freq;
	while(1){
		get_params_from_manager(manager_socket, &frequency1, &frequency2, &batch, &max_freq);	
		printf("Frequency 1: %s\nFrequency 2: %s\nBatch: %s\nMax Frequency Size: %s\n\n", frequency1, frequency2, batch, max_freq);

		params1.frequency=atof(frequency1);
		params1.batch=atoi(batch);
		params1.max_frequency=atoi(max_freq);
		
		params2.frequency=atof(frequency2);
		params2.batch=atoi(batch);
		params2.max_frequency=atoi(max_freq);
		
		//Record start time of sensor data gathering
    		gettimeofday(&start_time, NULL);
		
		//create threads
		pthread_t thread1, thread2;
		pthread_create(&thread1, NULL, thread_gather_archives, (void *)&params1);
		pthread_create(&thread2, NULL, thread_gather_archives, (void *)&params2);
		
		//join threads	
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		
		//After the sensor collection threads have finished begin the 
		//Merger thread and send the merged CSV to the manager
		extract_archive(manager_socket);
		
		//RECORD END TIME OF FILE RECEIVING PROCESS
    		gettimeofday(&end_time, NULL);
    		execution_time = (double)(end_time.tv_sec - start_time.tv_sec) +
               (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
		
		printf("Execution time: %.2f\n", execution_time);
	}
	return 0;
}



