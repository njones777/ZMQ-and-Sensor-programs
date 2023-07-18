C = cc

all: archivist catalog 

archivist: archival.c
	        $(CC) -o archivist archival.c -I./ -lzmq -lcrypto -pthread

catalog: t1.c
	        $(CC) -o catalog t1.c -I./ -lzmq -lcrypto -pthread

clean:
	        rm archivist catalog
