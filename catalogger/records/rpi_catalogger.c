#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <catalogger.h>

// Replace this with the archivists address
#define ARCHIVIST_ADDRESS "tcp://169.254.2.52:8887" 
// Path of CSV file to send to archivist
#define CSV_PATH "../CSVs/fft_radio.csv"

//global flag to indicate if ctrl+c was pressed
volatile sig_atomic_t ctrlc_pressed=0;

//signal handler function for SIGNINT (ctrl+c)
void sigint_handler(int signum){ctrlc_pressed=1;}

int main() {

    //install the signal handler
    signal(SIGINT, sigint_handler);

    // Create a ZeroMQ context and socket
    void *context = zmq_ctx_new();
    void *sender = connect_to_archivist(context, ARCHIVIST_ADDRESS);
    //printf("%s\n", ARCHIVIST_ADDRESS);
    if (sender == NULL){zmq_close(sender); zmq_ctx_destroy(context); return 1;}
    
    char frequency[10];
    char batch[4];
    int ibatch;
    float ffrequency;
    //receive frequency
    zmq_recv(sender, frequency, sizeof(frequency), 0);
    //receive batch size
    zmq_recv(sender, batch, sizeof(batch), 0);
    
    //convert batch to integer
    ibatch=atoi(batch);
    //convert frequency to float
    ffrequency=atof(frequency);
    
    char command[32];
    sprintf(command, "bash watch.sh %.1f", ffrequency);
    //start bash script
    int i=0;
    for(i;i<ibatch;i++){
    	if (ctrlc_pressed){break;}
    	system(command);
    	//printf("iteration complete\n");
    	int result = send_file_to_archivist(sender, CSV_PATH);
    	system("rm ../CSVs/fft_radio.csv");}
    
    
    // Cleanup
    zmq_close(sender);
    zmq_ctx_destroy(context);

    return 0;
}
