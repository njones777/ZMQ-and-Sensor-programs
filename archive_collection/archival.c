#include <zmq.h>
#include <stdio.h>
#include <pthread.h>
#include <archive.h>

#define RPI1_port "tcp://127.0.0.1:8888"

//#define RPI2_port "tcp://127.0.0.1:8889"




int main(){
	//create threads
	pthread_t thread1;
	pthread_create(&thread1, NULL, thread_gather_archives, (void *)RPI1_port);
	//pthread_create(&thread2, NULL, thread_gather_archives, (void *)RPI2_port);
	
	//wait for threads to finish, even though they won't
	pthread_join(thread1, NULL);
	//pthread_join(thread2, NULL);
	
	return 0;
}



