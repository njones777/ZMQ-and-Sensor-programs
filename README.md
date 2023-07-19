### Why:
---
The purpose of these programs were to provide the my team with a means of 
sending files and to help test bandwidth, latency and, program execution time
across multipel different types of devices. Such devices included, Raspberry Pis,
HPC systems, laptops, and micro controllers. These programs took advantage of [ZMQ](https://zeromq.org/),
a high-peformance messaging library that provides sockets and patterns for building
distributed and scalable applications. It helped to simply the complexity of our projects
interpocess communiction and networking by providing a lightweigh and flexible framework.

### Key Terms:
---
**RF** - Radio Frequency

**FFT** - An algorithm that computes the discrete Fourier transform (DFT) of a sequence, or its inverse (IDFT)

### Directory descriptions:
---
+ **archiver/** - Contains file and directories to run the archiver program. The archiver is a file collection program that is always waiting for sensors to send it CSV files produced from RF signal collection that has been turned into a CSV though a FFT program. 
+ **catalogger/** - This contains files and directories to run the catalogger program. The catalogger is a data collection program that will routinley collect RF singal data run a FFT program on it to convert it to a CSV file and send it to the catalogger
+ **benchmarking/** - This directory contains programs to test the bandwidth of multiples devices using ZMQ to transfer files of increasing size. During our project we used these programs to help find bottlenecks within our sensor network in order to optimize it for practical use. 

