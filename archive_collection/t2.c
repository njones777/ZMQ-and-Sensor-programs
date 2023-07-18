#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#define SERVER_ADDRESS "tcp://127.0.0.1:8889" // Replace this with the server address

int main() {
    // Create a ZeroMQ context and socket
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_PAIR);

    // Connect to the server
    int rc = zmq_connect(socket, SERVER_ADDRESS);
    if (rc != 0) {
        perror("Error connecting to the server");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return 1;
    }

    // Send a request to the server
    const char *request_msg = "Hello, server!"; // Replace this with your request message
    zmq_send(socket, request_msg, strlen(request_msg), 0);
    printf("Sent: %s\n", request_msg);

    

    // Cleanup
    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
