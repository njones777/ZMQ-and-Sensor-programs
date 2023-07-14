The purpose of these programs were to provide the my team with a means of 
sending files and to help test bandwidth, latency and, program execution time
across multipel different types of devices. Such devices included, raspberry pis,
HPC systems, laptops, and micro controllers. These programs took advantage of [ZMQ](https://zeromq.org/),
a high-peformance messaging library that provides sockets and patterns for building
distributed and scalable applications. It helped to simply the complexity of our projects
interpocess communiction and networking by providing a lightweigh and flexible framework.

### Progam descriptions:
---
+ **request.c** - sends request for a file to a listening supplicant in order to receive a file
+ **supplicant.c** - waits and listens for a file request from a requester in order to provide it with a file or files
 if you are doing a muli file-bandwidth test

### Syntax:
+ **Make executables:** ``` Make ```
+ **Start supplicant and wait for request(s):** ``` ./zmq_sup ```
+ **Send a file request to the supplicant:** ```./zmq_req 192.168.0.1 /path/to/file/on/supplicant /local/path/to/write/to```
   + The requester expects the ip address of the supplicant, the path of the file on the supplicant you are requesting
     and the local path you watn the requester to write to.
