/*
 * pcimem.c: Simple program to read/write from/to a pci device from userspace.
 *
 *  Copyright (C) 2010, Bill Farrow (bfarrow@beyondelectronics.us)
 *
 *  Based on the devmem2.c code
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <pcimem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>

#include <sys/mman.h>
#include <sys/stat.h>

#define PRINT_ERROR \
	do { \
		fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
		__LINE__, __FILE__, errno, strerror(errno)); exit(1); \
	} while(0)


int fileSize(int fd) {
   struct stat s;
   if (fstat(fd, &s) == -1) {
      int saveErrno = errno;
      fprintf(stderr, "fstat(%d) returned errno=%d.", fd, saveErrno);
      return(-1);
   }
   return(s.st_size);
}

void pcimem_test(int some)
{
	printf("*** pcimem_test entered.\n");		
}

void pcimem_init(Pcimem *self)
{	
	char* filename = "/dev/ixia/chassis/pcic";

    if((self->fd = open(filename, O_RDWR | O_SYNC)) == -1)
	{		
	 	PRINT_ERROR;
	} else 
	{
	    printf("%s opened.\n", filename);		
		self->map_size = fileSize(self->fd);
	    printf("mapsize is 0x%x, page size is %ld\n", self->map_size, sysconf(_SC_PAGE_SIZE));
	    fflush(stdout);
	}


    // Map one page 
    printf("mmap(%d, %d, 0x%x, 0x%x, %d)\n", 0, self->map_size, PROT_READ | PROT_WRITE, MAP_SHARED, self->fd);

    self->map_base = mmap(0, self->map_size, PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, 0);
    if(self->map_base == (void *) -1) PRINT_ERROR;
    printf("PCI Memory mapped to address 0x%08lx.\n", (unsigned long) self->map_base);
    fflush(stdout);
}

void pcimem_unmmap(Pcimem *self)
{
	if(munmap(self->map_base, self->map_size) == -1) PRINT_ERROR;
    close(self->fd);
}

uint64_t read64(Pcimem *self, uint64_t address)
{
	uint64_t read_result = 0xdeadbeef;
	void* virt_addr = self->map_base + address;
	read_result = *((uint64_t *) virt_addr);
	return read_result;
}

void write64(Pcimem *self, uint64_t address, uint64_t data)
{
	uint64_t read_result = 0xdeadbeef;
	void* virt_addr = self->map_base + address;
	*((uint64_t *) virt_addr) = data;
}


