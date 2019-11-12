/*
 *	The PCIMem Read and Write
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#ifndef PCIMEM
#define PCIMEM

#include <stdint.h>
#include <stddef.h>

typedef struct {
	int fd;
    int  map_size;
	void * map_base;
} Pcimem;
// int fd;
// void *map_base;
// void *virt_addr;
// int map_size;


void pcimem_init(Pcimem *self);
void pcimem_unmmap(Pcimem *self);
void testpcimem();
uint64_t read64(Pcimem *self, uint64_t address);
void write64(Pcimem *self, uint64_t address, uint64_t data);
#endif