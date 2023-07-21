#include <zmq.h>
#include <stdio.h>
#include <pthread.h>
#include <archive.h>

#define RPI1_port "tcp://169.254.2.52:8887"
#define RPI2_port "tcp://169.254.2.52:8886"




int main(){
	//create threads
	pthread_t thread1, thread2, thread3;
	pthread_create(&thread1, NULL, thread_gather_archives, (void *)RPI1_port);
	pthread_create(&thread2, NULL, thread_gather_archives, (void *)RPI2_port);
	pthread_create(&thread3, NULL, extract_archive, NULL);
	
	//wait for threads to finish, even though they won't
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL); 
	
	return 0;
}



