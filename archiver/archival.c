#include <zmq.h>
#include <stdio.h>
#include <pthread.h>
#include <archive.h>


#define RPI1_port "tcp://169.254.2.52:8887"
#define RPI2_port "tcp://169.254.2.52:8886"



int main(){
	struct RPI_params params1, params2;
	params1.port=(char *)RPI1_port;
	params2.port=(char *)RPI2_port;
	void *manager_socket=connect_to_manager();
	//empty variables for frequency and batch in structs
	char frequency[10];
	char batch[4];
	get_params_from_manager(manager_socket, frequency, batch);	
	printf("Frequency: %s, Batch: %s\n", frequency, batch);
		
		//Initialize the struct
		//initializeStruct(pi_params);

		//create threads
		//pthread_t thread2;
		//pthread_t thread1, thread2, thread3;
		//pthread_create(&thread1, NULL, thread_gather_archives, (void *)&params1);
		//pthread_create(&thread2, NULL, thread_gather_archives, (void *)&params2);
		//pthread_create(&thread3, NULL, extract_archive, NULL);
		
		//wait for threads to finish, even though they won't
		//pthread_join(thread1, NULL);
		//pthread_join(thread2, NULL);
		//pthread_join(thread3, NULL); */
	
	
	return 0;
}



