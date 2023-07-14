C = cc

all: requester supplicant

requester: request.c
	        $(CC) -o zmq_req request.c -I./ -lzmq

supplicant: supplicant.c
	        $(CC) -o zmq_sup supplicant.c -I./ -lzmq

clean:
	        rm zmq_req zmq_sup
