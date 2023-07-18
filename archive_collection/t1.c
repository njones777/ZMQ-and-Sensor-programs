#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <catalogger.h>

#define ARCHIVIST_ADDRESS "tcp://127.0.0.1:8888" // Replace this with the server address
//#define CSV_PATH "home/pi/Desktop/rf_automation_collection/fft_audio.csv"
#define CSV_PATH "/home/nmjones/fft_audio.csv"

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
