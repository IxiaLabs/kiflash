/*
 *	The KIFLASH Read and Write
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#ifndef KIFLASH
#define KIFLASH

#include <stdint.h>
#include <stddef.h>
#include<stdio.h> 

typedef struct {
	FILE* fd;
	int NoOfSector;
	int NoOfPage;
	uint32_t baseAddr;
	uint32_t eraseStartAddr;
	uint32_t eraseEndAddr;
	char* path;
	char* filename;
} Kiflash;


// int fd;
// void *map_base;
// void *virt_addr;
// int map_size;


int kiflash_init();
int kiflash_program_user();
void kiflash_test();
#endif