/*
 * IP2Location C library is distributed under LGPL version 3
 * Copyright (c) 2013 IP2Location.com. support at ip2location dot com 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not see <http://www.gnu.org/licenses/>.
 *
 */ 

#ifdef WIN32
#include <winsock2.h>
#else
#include <stdint.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#endif


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>


#include "IP2Location.h"
#include "IP2Loc_DBInterface.h"

#define IP2LOCATION_SHM "/IP2location_Shm"
#define MAP_ADDR 4194500608

//Static variables
static enum IP2Location_mem_type DB_access_type = IP2LOCATION_FILE_IO;
static void *cache_shm_ptr; 
#ifndef WIN32
static int32_t shm_fd;
#else
#ifdef WIN32
HANDLE shm_fd;
#endif
#endif

//Static functions
static int32_t IP2Location_DB_Load_to_mem(FILE *filehandle, void *cache_shm_ptr, int64_t size);  

//Description: set the DB access method as memory cache and read the file into cache
int32_t IP2Location_DB_set_memory_cache(FILE *filehandle)
{
	struct stat statbuf;
	DB_access_type = IP2LOCATION_CACHE_MEMORY;
	if(fstat(fileno(filehandle), &statbuf) == -1) {
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}

	if ( (cache_shm_ptr = malloc(statbuf.st_size + 1)) == NULL ) {
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;	
	}
	if( IP2Location_DB_Load_to_mem(filehandle, cache_shm_ptr, statbuf.st_size) == -1 ) {
		DB_access_type = IP2LOCATION_FILE_IO;
		free(cache_shm_ptr);
		return -1;	
	}
	return 0;
}

//Description: set the DB access method as shared memory
#ifndef WIN32
int32_t IP2Location_DB_set_shared_memory(FILE *filehandle)
{
	struct stat statbuf;
	int32_t DB_loaded = 1;
	void *addr = (void*)MAP_ADDR;

	DB_access_type = IP2LOCATION_SHARED_MEMORY;

	if ( ( shm_fd = shm_open(IP2LOCATION_SHM, O_RDWR | O_CREAT | O_EXCL, 0777)) != -1 ){
		DB_loaded = 0; 
	}
	else if ((shm_fd = shm_open(IP2LOCATION_SHM, O_RDWR , 0777)) == -1 ) { 
			DB_access_type = IP2LOCATION_FILE_IO;
			return -1;
	}
	if(fstat(fileno(filehandle), &statbuf) == -1) {
		close(shm_fd);
		if( DB_loaded == 0 )
			shm_unlink(IP2LOCATION_SHM);
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}

	if( DB_loaded == 0 && ftruncate(shm_fd, statbuf.st_size + 1) == -1) {
		close(shm_fd);
		shm_unlink(IP2LOCATION_SHM);
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}
	
	cache_shm_ptr = mmap(addr, statbuf.st_size + 1, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (cache_shm_ptr == (void *) -1) {
		close(shm_fd);
		if( DB_loaded == 0 )
			shm_unlink(IP2LOCATION_SHM);
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}
	if( DB_loaded == 0 ) {
		if ( IP2Location_DB_Load_to_mem(filehandle, cache_shm_ptr, statbuf.st_size) == -1 ) {
			munmap(cache_shm_ptr, statbuf.st_size);
			close(shm_fd);
			shm_unlink(IP2LOCATION_SHM);
			DB_access_type = IP2LOCATION_FILE_IO;
			return -1;	
		}
	}
	return 0;
}
#else
#ifdef WIN32
int32_t IP2Location_DB_set_shared_memory(FILE *filehandle)
{
	struct stat statbuf;
	int32_t DB_loaded = 1;

	DB_access_type = IP2LOCATION_SHARED_MEMORY;

	if(fstat(fileno(filehandle), &statbuf) == -1) {
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}

	shm_fd = CreateFileMapping(
                 INVALID_HANDLE_VALUE,
                 NULL,
                 PAGE_READWRITE,
                 0,
                 statbuf.st_size + 1,
                 TEXT(IP2LOCATION_SHM));
	if(shm_fd == NULL) {
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}

    DB_loaded = (GetLastError() == ERROR_ALREADY_EXISTS);

	cache_shm_ptr = MapViewOfFile( 
			shm_fd,
			FILE_MAP_WRITE,
			0, 
			0,
			0);

	if(cache_shm_ptr == NULL) {
		UnmapViewOfFile(cache_shm_ptr); 
		DB_access_type = IP2LOCATION_FILE_IO;
		return -1;
	}
	
	if( DB_loaded == 0 ) {
		if ( IP2Location_DB_Load_to_mem(filehandle, cache_shm_ptr, statbuf.st_size) == -1 ) {
			UnmapViewOfFile(cache_shm_ptr); 
			CloseHandle(shm_fd);
			DB_access_type = IP2LOCATION_FILE_IO;
			return -1;	
		}
	}
	return 0;
}
#endif
#endif

//Load the DB file into shared/cache memory  
int32_t IP2Location_DB_Load_to_mem(FILE *filehandle, void *memory, int64_t size)
{
	fseek(filehandle, SEEK_SET, 0);
	if ( fread(memory, size, 1, filehandle) != 1 )
		return -1;	
	return 0;
}

//Close the corresponding memory, based on the opened option. 
int32_t IP2Location_DB_close(FILE *filehandle)
{
	struct stat statbuf;
	if ( filehandle != NULL )
		fclose(filehandle);	
	if ( DB_access_type == IP2LOCATION_CACHE_MEMORY ) {
		if( cache_shm_ptr != NULL )
			free(cache_shm_ptr);
	}
	else if ( DB_access_type == IP2LOCATION_SHARED_MEMORY ) { 
		if( cache_shm_ptr != NULL ) {
#ifndef	WIN32
			if(fstat(fileno(filehandle), &statbuf) == 0) {
				munmap(cache_shm_ptr, statbuf.st_size);
			}
			close(shm_fd);
#else
#ifdef WIN32
			UnmapViewOfFile(cache_shm_ptr); 
			CloseHandle(shm_fd);
#endif
#endif
		}
	}
	DB_access_type = IP2LOCATION_FILE_IO;
	return 0;
}

#ifndef	WIN32
void IP2Location_DB_del_shm()
{
	shm_unlink(IP2LOCATION_SHM);
}
#else
#ifdef WIN32
void IP2Location_DB_del_shm()
{
}
#endif
#endif

struct in6_addr_local IP2Location_readIPv6Address(FILE *handle, uint32_t position) 
{
	int i,j;
	struct in6_addr_local addr6;
	for( i = 0, j = 15; i < 16; i++, j-- )
	{
		addr6.u.addr8[i] = IP2Location_read8(handle, position + j);
	}
	return addr6; 
}

uint32_t IP2Location_read32(FILE *handle, uint32_t position)
{
	uint8_t byte1 = 0;
	uint8_t byte2 = 0;
	uint8_t byte3 = 0;
	uint8_t byte4 = 0;
	uint8_t *cache_shm = cache_shm_ptr;
	
	//Read from file	
	if (DB_access_type == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&byte1, 1, 1, handle); 
		fread(&byte2, 1, 1, handle); 
		fread(&byte3, 1, 1, handle); 
		fread(&byte4, 1, 1, handle); 
	}
	else
	{
		byte1 = cache_shm[ position - 1 ]; 
		byte2 = cache_shm[ position ]; 
		byte3 = cache_shm[ position + 1 ]; 
		byte4 = cache_shm[ position + 2 ]; 
	}
	return ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | (byte1));
}

uint8_t IP2Location_read8(FILE *handle, uint32_t position)
{	
	uint8_t ret = 0;
	uint8_t *cache_shm = cache_shm_ptr;

	if (DB_access_type == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&ret, 1, 1, handle); 
	}
	else
	{
		ret = cache_shm[ position - 1 ]; 
	}		
	return ret;
}

char *IP2Location_readStr(FILE *handle, uint32_t position)
{
	uint8_t size = 0;
	char *str = 0;
	uint8_t *cache_shm = cache_shm_ptr;

	if (DB_access_type == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position, 0);
		fread(&size, 1, 1, handle); 
		str = (char *)malloc(size+1);
		memset(str, 0, size+1);
		fread(str, size, 1, handle); 
	}
	else {
		size = cache_shm[ position ]; 
		str = (char *)malloc(size+1);
		memset(str, 0, size+1);
		memcpy((void*) str, (void*)&cache_shm[ position + 1 ], size); 
	}
	return str;
}

float IP2Location_readFloat(FILE *handle, uint32_t position)
{
	float ret = 0.0;
	uint8_t *cache_shm = cache_shm_ptr;

#if defined(_SUN_) || defined(__powerpc__) || defined(__ppc__) || defined(__ppc64__) || defined(__powerpc64__)
	char * p = (char *) &ret;
	
	/* for SUN SPARC, have to reverse the byte order */
	if (DB_access_type == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position-1, 0);
		fread(p+3, 1, 1, handle);
		fread(p+2, 1, 1, handle);
		fread(p+1, 1, 1, handle);
		fread(p,   1, 1, handle);
	}
	else {
		*(p+3) = cache_shm[ position - 1 ]; 
		*(p+2) = cache_shm[ position ]; 
		*(p+1) = cache_shm[ position + 1 ]; 
		*(p)   = cache_shm[ position + 2 ]; 
	}
#else
	if (DB_access_type == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position-1, 0);
		fread(&ret, 4, 1, handle); 
	}
	else {
		memcpy((void*) &ret, (void*)&cache_shm[ position - 1 ], 4); 
	}
#endif
	return ret;
}


