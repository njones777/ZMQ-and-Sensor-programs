#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <catalogger.h>

// Replace this with the archivists address
#define ARCHIVIST_ADDRESS "tcp://169.254.2.52:8888" 
// Path of CSV file to send to archivist
#define CSV_PATH "CSVs/fft_audio.csv"


int main() {
    // Create a ZeroMQ context and socket
    void *context = zmq_ctx_new();
    void *sender = connect_to_archivist(context, ARCHIVIST_ADDRESS);
    printf("%s\n", ARCHIVIST_ADDRESS);
    if (sender == NULL){zmq_close(sender); zmq_ctx_destroy(context); return 1;}

    int result = send_file_to_archivist(sender, CSV_PATH);
    
    // Cleanup
    zmq_close(sender);
    zmq_ctx_destroy(context);

    return 0;
}
