C = cc

all: requester supplicant

requester: request.c
	        $(CC) -o zmq_req request.c -I./ -lzmq -lcrypto

supplicant: supplicant.c
	        $(CC) -o zmq_sup supplicant.c -I./ -lzmq -lcrypto

clean:
	        rm zmq_req zmq_sup
