#include <zmq.h>
#include <stdio.h>
#include <pthread.h>
#include <archive.h>
#include <stdlib.h>


#define RPI1_port "tcp://169.254.2.52:8887"
#define RPI2_port "tcp://169.254.2.52:8886"



int main(){
	struct RPI_params params1, params2;
	params1.port=(char *)RPI1_port;
	params2.port=(char *)RPI2_port;
	void *manager_socket=connect_to_manager();
	//empty variables for frequency and batch in structs
	char *frequency;
	char *batch;
	while(1){
		get_params_from_manager(manager_socket, &frequency, &batch);	
		printf("Frequency: %s\nBatch: %s\n", frequency, batch);
		
		params2.frequency=atof(frequency);
		params2.batch=atoi(batch);
		params1.frequency=atof(frequency);
		params1.batch=atoi(batch);
		
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
		//zmq_close(manager_socket);
		printf("COMPLETED\n");
	}
	return 0;
}



