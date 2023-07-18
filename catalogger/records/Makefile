C = cc

all:  catalog 

catalog: rpi_catalogger.c
	        $(CC) -o catalog rpi_catalogger.c -I./ -lzmq -lcrypto

clean:
	        rm catalog
