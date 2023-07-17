#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <openssl/md5.h>

int calc_md5_sum(const char* filename, unsigned char* checksum_str){
	FILE* file = fopen(filename, "rb");
	if(file==NULL){printf("Error opening file: %s\n", filename);return 0;}
	
	//md5_context structre of tyep MD5_CTX, represent sthe context or state of the MD5 calcualation
	MD5_CTX md5_context;
	//initializes the MD5 context by setting its internal state to the inital vlaues required
	//for the MD5 algorithm
	MD5_Init(&md5_context);
	
	unsigned char buffer[4096];
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) != 0) {
        MD5_Update(&md5_context, buffer, bytes_read);}
	
	unsigned char checksum[MD5_DIGEST_LENGTH];
	MD5_Final(checksum, &md5_context);
	
	fclose(file);
	
	//convert checksum bytes to hexadeciaml string
	for (int i=0; i< MD5_DIGEST_LENGTH; i++){
		sprintf(&checksum_str[i*2], "%02x", checksum[i]);
	}
	return 1;
}
#endif
