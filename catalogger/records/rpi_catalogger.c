#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <catalogger.h>

// Replace this with the archivists address
#define ARCHIVIST_ADDRESS "tcp://169.254.2.52:8887" 
// Path of CSV file to send to archivist
#define CSV_PATH "../CSVs/fft_radio.csv"

int main() {

    // Create a ZeroMQ context and socket
    void *context = zmq_ctx_new();
    void *sender = connect_to_archivist(context, ARCHIVIST_ADDRESS);
    //printf("%s\n", ARCHIVIST_ADDRESS);
    if (sender == NULL){zmq_close(sender); zmq_ctx_destroy(context); return 1;}
    while(1){
	    char frequency[10];
	    char batch[4];
	    char max_frequency_size[6];
	    float ffrequency;
	    int ibatch;
	    int max_freq;
	   
	    //receive frequency
	    zmq_recv(sender, frequency, sizeof(frequency), 0);
	    //receive batch size
	    zmq_recv(sender, batch, sizeof(batch), 0);
	    //receive max frequency size
	    zmq_recv(sender, max_frequency_size, sizeof(max_frequency_size), 0);
	    
	    
	    //convert frequency to float
	    ffrequency=atof(frequency);
	    //convert batch to integer
	    ibatch=atoi(batch);
	    //convert max frequency to integer
	    max_freq=atoi(max_frequency_size);
	    
	    
	    //generate command to run rtl_fm and to convert run fft on data
	    char command[32];
	    sprintf(command, "bash watch.sh %.1f %d", ffrequency, max_freq);
	    //start bash script
	    int i=0;
	    for(i;i<ibatch;i++){
	    	system(command);
	    	//printf("iteration complete\n");
	    	int result = send_file_to_archivist(sender, CSV_PATH);
	    	system("rm ../CSVs/fft_radio.csv");}
    } 
    // Cleanup
    zmq_close(sender);
    zmq_ctx_destroy(context);

    return 0;
}
