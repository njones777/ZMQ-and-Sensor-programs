C = cc

all: archivist

archivist: archival.c
	        $(CC) -o archivist archival.c -I./ -lzmq -lcrypto -pthread
clean:
	        rm archivist
