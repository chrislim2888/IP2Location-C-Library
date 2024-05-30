/*
 * IP2Location C library is distributed under MIT license
 * Copyright (c) 2013-2024 IP2Location.com. support at ip2location dot com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the MIT license
 */

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <stdint.h>
	#include <strings.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <sys/mman.h>
#endif

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>

#include "IP2Location.h"

#ifdef _WIN32
	#define PACKAGE_VERSION MACROSTR(IP2LOCATION_API_VERSION)
	#include <tchar.h>
#else
	#include "../config.h"
#endif

#define	CHECK_MODE(bf, bit) ((bf & (1<<(bit))) != 0)
#define	SHIFT_MODE(bit) (1<<(bit))

typedef struct ip_container {
	uint32_t version;
	uint32_t ipv4;
	struct in6_addr ipv6;
} ip_container;

uint8_t COUNTRY_POSITION[27]			= {0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2};
uint8_t REGION_POSITION[27]				= {0,  0,  0,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3};
uint8_t CITY_POSITION[27]				= {0,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4};
uint8_t LATITUDE_POSITION[27]			= {0,  0,  0,  0,  0,  5,  5,  0,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5};
uint8_t LONGITUDE_POSITION[27]			= {0,  0,  0,  0,  0,  6,  6,  0,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6};
uint8_t ZIPCODE_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  0,  7,  7,  7,  0,  7,  0,  7,  7,  7,  0,  7,  7,  7};
uint8_t TIMEZONE_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  7,  8,  8,  8,  7,  8,  0,  8,  8,  8,  0,  8,  8,  8};
uint8_t ISP_POSITION[27]				= {0,  0,  3,  0,  5,  0,  7,  5,  7,  0,  8,  0,  9,  0,  9,  0,  9,  0,  9,  7,  9,  0,  9,  7,  9,  9,  9};
uint8_t DOMAIN_POSITION[27]				= {0,  0,  0,  0,  0,  0,  0,  6,  8,  0,  9,  0, 10,  0, 10,  0, 10,  0, 10,  8, 10,  0, 10,  8, 10, 10, 10};
uint8_t NETSPEED_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 11,  0, 11,  8, 11,  0, 11,  0, 11,  0, 11, 11, 11};
uint8_t IDDCODE_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 12,  0, 12,  0, 12,  9, 12,  0, 12, 12, 12};
uint8_t AREACODE_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 13,  0, 13,  0, 13, 10, 13,  0, 13, 13, 13};
uint8_t WEATHERSTATIONCODE_POSITION[27]	= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 14,  0, 14,  0, 14,  0, 14, 14, 14};
uint8_t WEATHERSTATIONNAME_POSITION[27]	= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 15,  0, 15,  0, 15,  0, 15, 15, 15};
uint8_t MCC_POSITION[27]				= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 16,  0, 16,  9, 16, 16, 16};
uint8_t MNC_POSITION[27]				= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 17,  0, 17, 10, 17, 17, 17};
uint8_t MOBILEBRAND_POSITION[27]		= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11, 18,  0, 18, 11, 18, 18, 18};
uint8_t ELEVATION_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11, 19,  0, 19, 19, 19};
uint8_t USAGETYPE_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 12, 20, 20, 20};
uint8_t ADDRESSTYPE_POSITION[27]		= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 21, 21};
uint8_t CATEGORY_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 22, 22};
uint8_t DISTRICT_POSITION[27]			= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 23};
uint8_t ASN_POSITION[27]				= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24};
uint8_t AS_POSITION[27]					= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25};

// Static variables
static int32_t is_in_memory = 0;
static enum IP2Location_lookup_mode lookup_mode = IP2LOCATION_FILE_IO; /* Set default lookup mode as File I/O */

// Variables
uint8_t* memory_pointer;

// Static functions
static int IP2Location_initialize(IP2Location *handler);
static int IP2Location_is_ipv4(char *ip);
static int IP2Location_is_ipv6(char *ip);
static int32_t IP2Location_load_database_into_memory(FILE *file, void *memory_pointer, int64_t size);
static IP2LocationRecord *IP2Location_new_record();
static IP2LocationRecord *IP2Location_get_record(IP2Location *handler, char *ip, uint32_t mode);
static IP2LocationRecord *IP2Location_get_ipv4_record(IP2Location *handler, uint32_t mode, ip_container parsed_ip);
static IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *handler, uint32_t mode, ip_container parsed_ip);
#ifdef IP2LOCATION_HIDDEN_INTERNALS
static struct in6_addr IP2Location_readIPv6Address(FILE *handle, uint32_t position);
static uint32_t IP2Location_read32(FILE *handle, uint32_t position);
static uint32_t IP2Location_read32_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
static struct in6_addr IP2Location_read128_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
static uint8_t IP2Location_read8(FILE *handle, uint32_t position);
static uint8_t IP2Location_read8_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
static char *IP2Location_read_string(FILE* handle, uint32_t position);
static char *IP2Location_readStr(FILE *handle, uint32_t position);
static float IP2Location_readFloat(FILE *handle, uint32_t position);
static float IP2Location_read_float(FILE* handle, uint32_t position);
static float IP2Location_read_float_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
static struct in6_addr IP2Location_read_ipv6_address(FILE* handle, uint32_t position);
static int32_t IP2Location_DB_set_file_io();
static int32_t IP2Location_DB_set_memory_cache(FILE *file);
static int32_t IP2Location_set_memory_cache(FILE* file);
static int32_t IP2Location_DB_set_shared_memory(FILE *file);
static int32_t IP2Location_set_shared_memory(FILE* file);
static int32_t IP2Location_DB_close(FILE *file);
static int32_t IP2Location_close_memory(FILE* file);
static void IP2Location_DB_del_shm();
#endif

#ifndef WIN32
static int32_t shm_fd;
#else
#ifdef WIN32
HANDLE shm_fd;
#endif
#endif

// Open IP2Location BIN database file
IP2Location *IP2Location_open(char *bin)
{
	FILE *f;
	IP2Location *handler;

	if ((f = fopen(bin, "rb")) == NULL) {
		printf("IP2Location library error in opening database %s.\n", bin);
		return NULL;
	}

	handler = (IP2Location *) calloc(1, sizeof(IP2Location));
	if (handler == NULL) {
		fclose(f);
		return NULL;
	}
	handler->file = f;

	IP2Location_initialize(handler);

	if (handler->product_code == 1) {
		return handler;
	}

	if (handler->database_year <= 20 && handler->product_code == 0) {
		return handler;
	}

	printf(IP2LOCATION_INVALID_BIN_DATABASE);
	fclose(f);
	free(handler);
	return NULL;
}

// Initialize database structures
static int IP2Location_initialize(IP2Location *handler)
{
	uint8_t buffer[64];
	uint32_t mem_offset = 1;

	if (lookup_mode == IP2LOCATION_FILE_IO) {
		fread(buffer, sizeof(buffer), 1, handler->file);
	}

	handler->database_type = IP2Location_read8_row((uint8_t*)buffer, 0, mem_offset);
	handler->database_column = IP2Location_read8_row((uint8_t*)buffer, 1, mem_offset);
	handler->database_year = IP2Location_read8_row((uint8_t*)buffer, 2, mem_offset);
	handler->database_month = IP2Location_read8_row((uint8_t*)buffer, 3, mem_offset);
	handler->database_day = IP2Location_read8_row((uint8_t*)buffer, 4, mem_offset);
	handler->database_count = IP2Location_read32_row((uint8_t*)buffer, 5, mem_offset);
	handler->database_address = IP2Location_read32_row((uint8_t*)buffer, 9, mem_offset);
	handler->ip_version = IP2Location_read32_row((uint8_t*)buffer, 13, mem_offset);
	handler->ipv4_database_count = IP2Location_read32_row((uint8_t*)buffer, 5, mem_offset);
	handler->ipv4_database_address = IP2Location_read32_row((uint8_t*)buffer, 9, mem_offset);
	handler->ipv6_database_count = IP2Location_read32_row((uint8_t*)buffer, 13, mem_offset);
	handler->ipv6_database_address = IP2Location_read32_row((uint8_t*)buffer, 17, mem_offset);
	handler->ipv4_index_base_address = IP2Location_read32_row((uint8_t*)buffer, 21, mem_offset);
	handler->ipv6_index_base_address = IP2Location_read32_row((uint8_t*)buffer, 25, mem_offset);
	handler->product_code = IP2Location_read8_row((uint8_t*)buffer, 29, mem_offset);
	handler->license_code = IP2Location_read8_row((uint8_t*)buffer, 30, mem_offset);
	handler->database_size = IP2Location_read32_row((uint8_t*)buffer, 31, mem_offset);

	return 0;
}

// This function to set the DB access type.
int32_t IP2Location_open_mem(IP2Location *handler, enum IP2Location_lookup_mode mode)
{
	// BIN database is not loaded
	if (handler == NULL) {
		return -1;
	}

	// Existing database already loaded into memory
	if (is_in_memory != 0) {
		return -1;
	}

	// Mark database loaded into memory
	is_in_memory = 1;

	if (mode == IP2LOCATION_FILE_IO) {
		return 0;
	} else if (mode == IP2LOCATION_CACHE_MEMORY) {
		return IP2Location_DB_set_memory_cache(handler->file);
	} else if (mode == IP2LOCATION_SHARED_MEMORY) {
		return IP2Location_DB_set_shared_memory(handler->file);
	} else {
		return -1;
	}
}

// Alias to IP2Location_open_mem()
int32_t IP2Location_set_lookup_mode(IP2Location *handler, enum IP2Location_lookup_mode mode)
{
	return IP2Location_open_mem(handler, mode);
}

// Close the IP2Location database file
uint32_t IP2Location_close(IP2Location *handler)
{
	is_in_memory = 0;

	if (handler != NULL) {
		IP2Location_DB_close(handler->file);
		free(handler);
	}

	return 0;
}

// Delete IP2Location shared memory if its present
void IP2Location_delete_shm()
{
	IP2Location_DB_del_shm();
}

// Alias to IP2Location_DB_del_shm()
void IP2Location_clear_memory()
{
	IP2Location_DB_del_shm();
}

// Alias to IP2Location_DB_del_shm()
void IP2Location_delete_shared_memory()
{
	IP2Location_DB_del_shm();
}

// Compare IPv6 address
static int ipv6_compare(struct in6_addr *addr1, struct in6_addr *addr2)
{
	int i, ret = 0;
	for (i = 0; i < 16; i++) {
		if (addr1->s6_addr[i] > addr2->s6_addr[i]) {
			ret = 1;
			break;
		} else if (addr1->s6_addr[i] < addr2->s6_addr[i]) {
			ret = -1;
			break;
		}
	}

	return ret;
}

// Alias to ipv6_compare()
static int IP2Location_ipv6_compare(struct in6_addr *addr1, struct in6_addr *addr2)
{
	return ipv6_compare(addr1, addr2);
}

// Parse IP address into binary address for lookup purpose
static ip_container IP2Location_parse_address(const char *ip)
{
	ip_container parsed;

	if (IP2Location_is_ipv4((char *) ip)) {
		// Parse IPv4 address
		parsed.version = 4;
		inet_pton(AF_INET, ip, &parsed.ipv4);
		parsed.ipv4 = htonl(parsed.ipv4);
	} else if (IP2Location_is_ipv6((char *) ip)) {
		// Parse IPv6 address
		inet_pton(AF_INET6, ip, &parsed.ipv6);

		// IPv4 Address in IPv6
		if (parsed.ipv6.s6_addr[0] == 0 && parsed.ipv6.s6_addr[1] == 0 && parsed.ipv6.s6_addr[2] == 0 && parsed.ipv6.s6_addr[3] == 0 && parsed.ipv6.s6_addr[4] == 0 && parsed.ipv6.s6_addr[5] == 0 && parsed.ipv6.s6_addr[6] == 0 && parsed.ipv6.s6_addr[7] == 0 && parsed.ipv6.s6_addr[8] == 0 && parsed.ipv6.s6_addr[9] == 0 && parsed.ipv6.s6_addr[10] == 255 && parsed.ipv6.s6_addr[11] == 255) {
			parsed.version = 4;
			parsed.ipv4 = (parsed.ipv6.s6_addr[12] << 24) + (parsed.ipv6.s6_addr[13] << 16) + (parsed.ipv6.s6_addr[14] << 8) + parsed.ipv6.s6_addr[15];
		}

		// 6to4 Address - 2002::/16
		else if (parsed.ipv6.s6_addr[0] == 32 && parsed.ipv6.s6_addr[1] == 2) {
			parsed.version = 4;
			parsed.ipv4 = (parsed.ipv6.s6_addr[2] << 24) + (parsed.ipv6.s6_addr[3] << 16) + (parsed.ipv6.s6_addr[4] << 8) + parsed.ipv6.s6_addr[5];
		}

		// Teredo Address - 2001:0::/32
		else if (parsed.ipv6.s6_addr[0] == 32 && parsed.ipv6.s6_addr[1] == 1 && parsed.ipv6.s6_addr[2] == 0 && parsed.ipv6.s6_addr[3] == 0) {
			parsed.version = 4;
			parsed.ipv4 = ~((parsed.ipv6.s6_addr[12] << 24) + (parsed.ipv6.s6_addr[13] << 16) + (parsed.ipv6.s6_addr[14] << 8) + parsed.ipv6.s6_addr[15]);
		}

		// Common IPv6 Address
		else {
			parsed.version = 6;
		}
	} else {
		// Invalid IP address
		parsed.version = -1;
	}

	return parsed;
}

// Get country code
IP2LocationRecord *IP2Location_get_country_short(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_COUNTRYSHORT));
}

// Get country name
IP2LocationRecord *IP2Location_get_country_long(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_COUNTRYLONG));
}

// Get the name of state/region
IP2LocationRecord *IP2Location_get_region(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_REGION));
}

// Get city name
IP2LocationRecord *IP2Location_get_city(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_CITY));
}

// Get ISP name
IP2LocationRecord *IP2Location_get_isp(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_ISP));
}

// Get latitude
IP2LocationRecord *IP2Location_get_latitude(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_LATITUDE));
}

// Get longitude
IP2LocationRecord *IP2Location_get_longitude(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_LONGITUDE));
}

// Get domain name
IP2LocationRecord *IP2Location_get_domain(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_DOMAINNAME));
}

// Get ZIP code
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_ZIPCODE));
}

// Get time zone
IP2LocationRecord *IP2Location_get_timezone(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_TIMEZONE));
}

// Get net speed
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_NETSPEED));
}

// Get IDD code
IP2LocationRecord *IP2Location_get_iddcode(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_IDDCODE));
}

// Get area code
IP2LocationRecord *IP2Location_get_areacode(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_AREACODE));
}

// Get weather station code
IP2LocationRecord *IP2Location_get_weatherstationcode(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_WEATHERSTATIONCODE));
}

// Get weather station name
IP2LocationRecord *IP2Location_get_weatherstationname(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_WEATHERSTATIONNAME));
}

// Get mobile country code
IP2LocationRecord *IP2Location_get_mcc(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_MCC));
}

// Get mobile national code
IP2LocationRecord *IP2Location_get_mnc(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_MNC));
}

// Get mobile carrier brand
IP2LocationRecord *IP2Location_get_mobilebrand(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_MOBILEBRAND));
}

// Get elevation
IP2LocationRecord *IP2Location_get_elevation(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_ELEVATION));
}

// Get usage type
IP2LocationRecord *IP2Location_get_usagetype(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_USAGETYPE));
}

// Get address type
IP2LocationRecord *IP2Location_get_addresstype(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_ADDRESSTYPE));
}

// Get category
IP2LocationRecord *IP2Location_get_category(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_CATEGORY));
}

// Get district
IP2LocationRecord *IP2Location_get_district(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_DISTRICT));
}

// Get ASN
IP2LocationRecord *IP2Location_get_asn(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_ASN));
}

// Get AS
IP2LocationRecord *IP2Location_get_as(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, SHIFT_MODE(IP2LOCATION_AS));
}

// Get all records of an IP address
IP2LocationRecord *IP2Location_get_all(IP2Location *handler, char *ip)
{
	return IP2Location_get_record(handler, ip, IP2LOCATION_ALL);
}

// fill the record fields with error message
static IP2LocationRecord *IP2Location_bad_record(const char *message)
{
	IP2LocationRecord *record = IP2Location_new_record();
	record->country_short = strdup(message);
	record->country_long = strdup(message);
	record->region = strdup(message);
	record->city = strdup(message);
	record->isp = strdup(message);
	record->domain = strdup(message);
	record->zipcode = strdup(message);
	record->timezone = strdup(message);
	record->netspeed = strdup(message);
	record->iddcode = strdup(message);
	record->areacode = strdup(message);
	record->weatherstationcode = strdup(message);
	record->weatherstationname = strdup(message);
	record->mcc = strdup(message);
	record->mnc = strdup(message);
	record->mobilebrand = strdup(message);
	record->usagetype = strdup(message);

	record->latitude = 0;
	record->longitude = 0;
	record->elevation = 0;

	record->address_type = strdup(message);
	record->category = strdup(message);
	record->district = strdup(message);
	record->asn = strdup(message);
	record->as = strdup(message);

	return record;
}

static IP2LocationRecord *IP2Location_read_record(IP2Location *handler, uint8_t* buffer, uint32_t mode, uint32_t mem_offset) {
	uint8_t database_type = handler->database_type;
	FILE *handle = handler->file;
	IP2LocationRecord *record = IP2Location_new_record();

	if (CHECK_MODE(mode, IP2LOCATION_COUNTRYSHORT) && (COUNTRY_POSITION[database_type] != 0)) {
		if (!record->country_short) {
			record->country_short = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (COUNTRY_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->country_short) {
			record->country_short = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_COUNTRYLONG) && (COUNTRY_POSITION[database_type] != 0)) {
		if (!record->country_long) {
			record->country_long = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (COUNTRY_POSITION[database_type] - 2), mem_offset) + 3);
		}
	} else {
		if (!record->country_long) {
			record->country_long = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_REGION) && (REGION_POSITION[database_type] != 0)) {
		if (!record->region) {
			record->region = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (REGION_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->region) {
			record->region = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_CITY) && (CITY_POSITION[database_type] != 0)) {
		if (!record->city) {
			record->city = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (CITY_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->city) {
			record->city = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_ISP) && (ISP_POSITION[database_type] != 0)) {
		if (!record->isp) {
			record->isp = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (ISP_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->isp) {
			record->isp = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_LATITUDE) && (LATITUDE_POSITION[database_type] != 0)) {
		record->latitude = IP2Location_read_float_row(buffer, 4 * (LATITUDE_POSITION[database_type] - 2), mem_offset);
	} else {
		record->latitude = 0.0;
	}

	if (CHECK_MODE(mode, IP2LOCATION_LONGITUDE) && (LONGITUDE_POSITION[database_type] != 0)) {
		record->longitude = IP2Location_read_float_row(buffer, 4 * (LONGITUDE_POSITION[database_type] - 2), mem_offset);
	} else {
		record->longitude = 0.0;
	}

	if (CHECK_MODE(mode, IP2LOCATION_DOMAINNAME) && (DOMAIN_POSITION[database_type] != 0)) {
		if (!record->domain) {
			record->domain = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (DOMAIN_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->domain) {
			record->domain = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_ZIPCODE) && (ZIPCODE_POSITION[database_type] != 0)) {
		if (!record->zipcode) {
			record->zipcode = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (ZIPCODE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->zipcode) {
			record->zipcode = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_TIMEZONE) && (TIMEZONE_POSITION[database_type] != 0)) {
		if (!record->timezone) {
			record->timezone = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (TIMEZONE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->timezone) {
			record->timezone = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_NETSPEED) && (NETSPEED_POSITION[database_type] != 0)) {
		if (!record->netspeed) {
			record->netspeed = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (NETSPEED_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->netspeed) {
			record->netspeed = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_IDDCODE) && (IDDCODE_POSITION[database_type] != 0)) {
		if (!record->iddcode) {
			record->iddcode = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (IDDCODE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->iddcode) {
			record->iddcode = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_AREACODE) && (AREACODE_POSITION[database_type] != 0)) {
		if (!record->areacode) {
			record->areacode = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (AREACODE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->areacode) {
			record->areacode = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_WEATHERSTATIONCODE) && (WEATHERSTATIONCODE_POSITION[database_type] != 0)) {
		if (!record->weatherstationcode) {
			record->weatherstationcode = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (WEATHERSTATIONCODE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->weatherstationcode) {
			record->weatherstationcode = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_WEATHERSTATIONNAME) && (WEATHERSTATIONNAME_POSITION[database_type] != 0)) {
		if (!record->weatherstationname) {
			record->weatherstationname = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (WEATHERSTATIONNAME_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->weatherstationname) {
			record->weatherstationname = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_MCC) && (MCC_POSITION[database_type] != 0)) {
		if (!record->mcc) {
			record->mcc = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (MCC_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->mcc) {
			record->mcc = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_MNC) && (MNC_POSITION[database_type] != 0)) {
		if (!record->mnc) {
			record->mnc = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (MNC_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->mnc) {
			record->mnc = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_MOBILEBRAND) && (MOBILEBRAND_POSITION[database_type] != 0)) {
		if (!record->mobilebrand) {
			record->mobilebrand = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (MOBILEBRAND_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->mobilebrand) {
			record->mobilebrand = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_ELEVATION) && (ELEVATION_POSITION[database_type] != 0)) {
		char * mem = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (ELEVATION_POSITION[database_type] - 2), mem_offset));
		record->elevation = atof(mem);
		free(mem);
	} else {
		record->elevation = 0.0;
	}

	if (CHECK_MODE(mode, IP2LOCATION_USAGETYPE) && (USAGETYPE_POSITION[database_type] != 0)) {
		if (!record->usagetype) {
			record->usagetype = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (USAGETYPE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->usagetype) {
			record->usagetype = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_ADDRESSTYPE) && (ADDRESSTYPE_POSITION[database_type] != 0)) {
		if (!record->address_type) {
			record->address_type = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (ADDRESSTYPE_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->address_type) {
			record->address_type = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_CATEGORY) && (CATEGORY_POSITION[database_type] != 0)) {
		if (!record->category) {
			record->category = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (CATEGORY_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->category) {
			record->category = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_DISTRICT) && (DISTRICT_POSITION[database_type] != 0)) {
		if (!record->district) {
			record->district = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (DISTRICT_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->district) {
			record->district = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_ASN) && (ASN_POSITION[database_type] != 0)) {
		if (!record->asn) {
			record->asn = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (ASN_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->asn) {
			record->asn = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	if (CHECK_MODE(mode, IP2LOCATION_AS) && (AS_POSITION[database_type] != 0)) {
		if (!record->as) {
			record->as = IP2Location_read_string(handle, IP2Location_read32_row(buffer, 4 * (AS_POSITION[database_type] - 2), mem_offset));
		}
	} else {
		if (!record->as) {
			record->as = strdup(IP2LOCATION_NOT_SUPPORTED);
		}
	}

	return record;
}

// Get record for a IPv6 from database
static IP2LocationRecord * IP2Location_get_ipv6_record(IP2Location *handler, uint32_t mode, ip_container parsed_ip) {
	FILE * handle = handler->file;
	uint32_t base_address = handler->ipv6_database_address;
	uint32_t database_column = handler->database_column;
	uint32_t ipv6_index_base_address = handler->ipv6_index_base_address;

	uint32_t low = 0;
	uint32_t high = handler->ipv6_database_count;
	uint32_t mid = 0;

	struct in6_addr ip_from;
	struct in6_addr ip_to;
	struct in6_addr ip_number;

	uint32_t column_offset = database_column * 4 + 12;
	uint32_t row_offset = 0;
	uint8_t full_row_buffer[200];
	uint8_t row_buffer[200];
	uint32_t full_row_size;
	uint32_t row_size;
	uint32_t mem_offset;

	ip_number = parsed_ip.ipv6;

	if (!high) {
		return NULL;
	}

	if (ipv6_index_base_address > 0) {
		uint32_t ipnum1 = (ip_number.s6_addr[0] * 256) + ip_number.s6_addr[1];
		uint32_t indexpos = ipv6_index_base_address + (ipnum1 << 3);

		uint8_t indexbuffer[8];
		if (lookup_mode == IP2LOCATION_FILE_IO) {
			fseek(handle, indexpos - 1, 0);
			fread(indexbuffer, sizeof(indexbuffer), 1, handle);
		}
		mem_offset = indexpos;
		low = IP2Location_read32_row((uint8_t*)indexbuffer, 0, mem_offset);
		high = IP2Location_read32_row((uint8_t*)indexbuffer, 4, mem_offset);
	}

	full_row_size = column_offset + 16;
	row_size = column_offset - 16;

	while (low <= high) {
		mid = (uint32_t)((low + high) >> 1);
		row_offset = base_address + (mid * column_offset);

		if (lookup_mode == IP2LOCATION_FILE_IO) {
			fseek(handle, row_offset - 1, 0);
			fread(&full_row_buffer, full_row_size, 1, handle);
		}
		mem_offset = row_offset;

		ip_from = IP2Location_read128_row((uint8_t *)full_row_buffer, 0, mem_offset);
		ip_to = IP2Location_read128_row((uint8_t *)full_row_buffer, column_offset, mem_offset);

		if ((IP2Location_ipv6_compare(&ip_number, &ip_from) >= 0) && (IP2Location_ipv6_compare(&ip_number, &ip_to) < 0)) {
			if (lookup_mode == IP2LOCATION_FILE_IO) {
				memcpy(&row_buffer, ((uint8_t*)full_row_buffer) + 16, row_size); // extract actual row data
			}
			return IP2Location_read_record(handler, (uint8_t *)row_buffer, mode, mem_offset + 16);
		} else {
			if (IP2Location_ipv6_compare(&ip_number, &ip_from) < 0) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}

	return NULL;
}

// Get record for a IPv4 from database
static IP2LocationRecord *IP2Location_get_ipv4_record(IP2Location *handler, uint32_t mode, ip_container parsed_ip) {
	FILE *handle = handler->file;
	uint32_t base_address = handler->ipv4_database_address;
	uint32_t database_column = handler->database_column;
	uint32_t ipv4_index_base_address = handler->ipv4_index_base_address;

	uint32_t low = 0;
	uint32_t high = handler->ipv4_database_count;
	uint32_t mid = 0;

	uint32_t ip_number;
	uint32_t ip_from;
	uint32_t ip_to;

	uint32_t column_offset = database_column * 4;
	uint32_t row_offset = 0;
	uint8_t full_row_buffer[200];
	uint8_t row_buffer[200];
	uint32_t full_row_size;
	uint32_t row_size;
	uint32_t mem_offset;

	ip_number = parsed_ip.ipv4;

	if (ip_number == (uint32_t) IP2LOCATION_MAX_IPV4_RANGE) {
		ip_number = ip_number - 1;
	}

	if (ipv4_index_base_address > 0) {
		uint32_t ipnum1n2 = (uint32_t) ip_number >> 16;
		uint32_t indexpos = ipv4_index_base_address + (ipnum1n2 << 3);

		uint8_t indexbuffer[8];
		if (lookup_mode == IP2LOCATION_FILE_IO) {
			fseek(handle, indexpos - 1, 0);
			fread(indexbuffer, sizeof(indexbuffer), 1, handle);
		}
		mem_offset = indexpos;
		low = IP2Location_read32_row((uint8_t*)indexbuffer, 0, mem_offset);
		high = IP2Location_read32_row((uint8_t*)indexbuffer, 4, mem_offset);
	}

	full_row_size = column_offset + 4;
	row_size = column_offset - 4;

	while (low <= high) {
		mid = (uint32_t)((low + high) >> 1);
		row_offset = base_address + (mid * column_offset);

		if (lookup_mode == IP2LOCATION_FILE_IO) {
			fseek(handle, row_offset - 1, 0);
			fread(&full_row_buffer, full_row_size, 1, handle);
		}
		mem_offset = row_offset;

		ip_from = IP2Location_read32_row((uint8_t*)full_row_buffer, 0, mem_offset);
		ip_to = IP2Location_read32_row((uint8_t*)full_row_buffer, column_offset, mem_offset);

		if ((ip_number >= ip_from) && (ip_number < ip_to)) {
			if (lookup_mode == IP2LOCATION_FILE_IO) {
				memcpy(&row_buffer, ((uint8_t*)full_row_buffer) + 4, row_size); // extract actual row data
			}
			return IP2Location_read_record(handler, (uint8_t *)row_buffer, mode, mem_offset + 4);
		} else {
			if (ip_number < ip_from) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	return NULL;
}

// Get the location data
static IP2LocationRecord *IP2Location_get_record(IP2Location *handler, char *ip, uint32_t mode) {
	ip_container parsed_ip = IP2Location_parse_address(ip);

	if (parsed_ip.version == 4) {
		return IP2Location_get_ipv4_record(handler, mode, parsed_ip);
	}
	if (parsed_ip.version == 6) {
		if (handler->ipv6_database_count == 0) {
			return IP2Location_bad_record(IP2LOCATION_IPV6_ADDRESS_MISSING_IN_IPV4_BIN);
		}

		return IP2Location_get_ipv6_record(handler, mode, parsed_ip);
	} else {
		return IP2Location_bad_record(IP2LOCATION_INVALID_IP_ADDRESS);
	}
}

// Initialize the record object
static IP2LocationRecord *IP2Location_new_record()
{
	IP2LocationRecord *record = (IP2LocationRecord *) calloc(1, sizeof(IP2LocationRecord));
	return record;
}

// Free the record object
void IP2Location_free_record(IP2LocationRecord *record) {
	if (record == NULL) {
		return;
	}

	free(record->city);
	free(record->country_long);
	free(record->country_short);
	free(record->domain);
	free(record->isp);
	free(record->region);
	free(record->zipcode);
	free(record->timezone);
	free(record->netspeed);
	free(record->iddcode);
	free(record->areacode);
	free(record->weatherstationcode);
	free(record->weatherstationname);
	free(record->mcc);
	free(record->mnc);
	free(record->mobilebrand);
	free(record->usagetype);
	free(record->address_type);
	free(record->category);
	free(record->district);
	free(record->asn);
	free(record->as);

	free(record);
}

// Check if address is IPv4
static int IP2Location_is_ipv4(char *ip)
{
	struct sockaddr_in sa;
	return inet_pton(AF_INET, ip, &sa.sin_addr);
}

// Check if address is IPv6
static int IP2Location_is_ipv6(char *ip)
{
	struct in6_addr result;
	return inet_pton(AF_INET6, ip, &result);
}

// Get API version numeric (Will deprecate in coming major version update)
unsigned long int IP2Location_api_version_num(void)
{
	return (IP2LOCATION_API_VERSION_NUMERIC);
}

// Alias to IP2Location_api_version_num()
unsigned long int IP2Location_api_version_number(void)
{
	return IP2Location_api_version_num();
}

// Get API version as string
char *IP2Location_api_version_string(void)
{
	static char version[64];
	sprintf(version, "%d.%d.%d", IP2LOCATION_API_VERSION_MAJOR, IP2LOCATION_API_VERSION_MINOR, IP2LOCATION_API_VERSION_RELEASE);
	return (version);
}

// Get library version as string
char *IP2Location_lib_version_string(void)
{
	return (PACKAGE_VERSION);
}

// Get BIN database version
char *IP2Location_bin_version(IP2Location *handler)
{
	if (handler == NULL) {
		return NULL;
	}

	static char version[64];

	sprintf(version, "%d-%d-%d", handler->database_year + 2000, handler->database_month, handler->database_day);

	return (version);
}

// Set to use memory caching
IP2LOCATION_STATIC int32_t IP2Location_DB_set_memory_cache(FILE *file)
{
	struct stat buffer;
	lookup_mode = IP2LOCATION_CACHE_MEMORY;

	if (fstat(fileno(file), &buffer) == -1) {
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	if ((memory_pointer = malloc(buffer.st_size + 1)) == NULL) {
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	if (IP2Location_load_database_into_memory(file, memory_pointer, buffer.st_size) == -1) {
		lookup_mode = IP2LOCATION_FILE_IO;
		free(memory_pointer);
		return -1;
	}

	return 0;
}

// Alias to IP2Location_DB_set_memory_cache()
IP2LOCATION_STATIC int32_t IP2Location_set_memory_cache(FILE *file)
{
	return IP2Location_DB_set_memory_cache(file);
}

// Return the shared-memory name
#ifndef WIN32
static const char *get_shm_name(void) {
	static char name[64] = "";

	if (!name[0]) {
		sprintf(name, "/%s_%ld", IP2LOCATION_SHM, (long)getpid());
	}
	return name;
}

// Set to use shared memory
IP2LOCATION_STATIC int32_t IP2Location_DB_set_shared_memory(FILE *file)
{
	struct stat buffer;
	int32_t is_dababase_loaded = 1;
	void *addr = (void*)IP2LOCATION_MAP_ADDR;

	lookup_mode = IP2LOCATION_SHARED_MEMORY;

	// New shared memory object is created
	if ((shm_fd = shm_open(get_shm_name(), O_RDWR | O_CREAT | O_EXCL, 0777)) != -1) {
		is_dababase_loaded = 0;
	}

	// Failed to create new shared memory object
	else if ((shm_fd = shm_open(get_shm_name(), O_RDWR , 0777)) == -1) {
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	if (fstat(fileno(file), &buffer) == -1) {
		close(shm_fd);

		if (is_dababase_loaded == 0) {
			shm_unlink(get_shm_name());
		}

		lookup_mode = IP2LOCATION_FILE_IO;

		return -1;
	}

	if (is_dababase_loaded == 0 && ftruncate(shm_fd, buffer.st_size + 1) == -1) {
		close(shm_fd);
		shm_unlink(get_shm_name());
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	memory_pointer = mmap(addr, buffer.st_size + 1, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if (memory_pointer == (void *) -1) {
		close(shm_fd);

		if (is_dababase_loaded == 0) {
			shm_unlink(get_shm_name());
		}

		lookup_mode = IP2LOCATION_FILE_IO;

		return -1;
	}

	if (is_dababase_loaded == 0) {
		if (IP2Location_load_database_into_memory(file, memory_pointer, buffer.st_size) == -1) {
			munmap(memory_pointer, buffer.st_size);
			close(shm_fd);
			shm_unlink(get_shm_name());
			lookup_mode = IP2LOCATION_FILE_IO;
			return -1;
		}
	}

	return 0;
}
#else
#ifdef WIN32
// Return a name for the shared-memory object
// For Windows, this depends on whether 'UNICODE' is defined
// (hence the use of 'TCHAR').
static const TCHAR *get_shm_name(void) {
	static TCHAR name[64] = _T("");

	if (!name[0]) {
		_sntprintf(name, sizeof(name)/sizeof(name[0]), _T("%s_%lu"),
                   _T(IP2LOCATION_SHM), GetProcessId(NULL));
	}
	return name;
}

IP2LOCATION_STATIC int32_t IP2Location_DB_set_shared_memory(FILE *file)
{
	struct stat buffer;
	int32_t is_dababase_loaded = 1;

	lookup_mode = IP2LOCATION_SHARED_MEMORY;

	if (fstat(fileno(file), &buffer) == -1) {
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	shm_fd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, buffer.st_size + 1, get_shm_name());

	if (shm_fd == NULL) {
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	is_dababase_loaded = (GetLastError() == ERROR_ALREADY_EXISTS);
	memory_pointer = MapViewOfFile(shm_fd, FILE_MAP_WRITE, 0, 0, 0);

	if (memory_pointer == NULL) {
		UnmapViewOfFile(memory_pointer);
		lookup_mode = IP2LOCATION_FILE_IO;
		return -1;
	}

	if (is_dababase_loaded == 0) {
		if (IP2Location_load_database_into_memory(file, memory_pointer, buffer.st_size) == -1) {
			UnmapViewOfFile(memory_pointer);
			CloseHandle(shm_fd);
			lookup_mode = IP2LOCATION_FILE_IO;
			return -1;
		}
	}

	return 0;
}
#endif
#endif

// Alias to IP2Location_DB_set_shared_memory()
IP2LOCATION_STATIC int32_t IP2Location_set_shared_memory(FILE *file)
{
	return IP2Location_DB_set_shared_memory(file);
}

// Load BIN file into memory
IP2LOCATION_STATIC int32_t IP2Location_load_database_into_memory(FILE *file, void *memory, int64_t size)
{
	fseek(file, 0, SEEK_SET);

	if (fread(memory, size, 1, file) != 1) {
		return -1;
	}

	return 0;
}

// Close the corresponding memory, based on the opened option
IP2LOCATION_STATIC int32_t IP2Location_DB_close(FILE *file)
{
	struct stat buffer;

	if (lookup_mode == IP2LOCATION_CACHE_MEMORY) {
		if (memory_pointer != NULL) {
			free(memory_pointer);
		}
	} else if (lookup_mode == IP2LOCATION_SHARED_MEMORY) {
		if (memory_pointer != NULL) {
#ifndef	WIN32
			if (fstat(fileno(file), &buffer) == 0) {
				munmap(memory_pointer, buffer.st_size);
			}

			close(shm_fd);
#else
#ifdef WIN32
			UnmapViewOfFile(memory_pointer);
			CloseHandle(shm_fd);
#endif
#endif
		}
	}

	if (file != NULL) {
		fclose(file);
	}

	lookup_mode = IP2LOCATION_FILE_IO;
	return 0;
}


// Alias to IP2Location_DB_close
IP2LOCATION_STATIC int32_t IP2Location_close_memory(FILE *file)
{
	return IP2Location_DB_close(file);
}

#ifndef	WIN32
// Remove shared memory object
IP2LOCATION_STATIC void IP2Location_DB_del_shm()
{
	shm_unlink(get_shm_name());
}
#else
#ifdef WIN32
IP2LOCATION_STATIC void IP2Location_DB_del_shm()
{
}
#endif
#endif

IP2LOCATION_STATIC struct in6_addr IP2Location_read128_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset)
{
	int i, j;
	struct in6_addr addr6;
	for (i = 0, j = 15; i < 16; i++, j--)
	{
		addr6.s6_addr[i] = IP2Location_read8_row(buffer, position + j, mem_offset);
	}
	return addr6;
}

IP2LOCATION_STATIC struct in6_addr IP2Location_readIPv6Address(FILE *handle, uint32_t position)
{
	int i, j;
	struct in6_addr addr6;

	for (i = 0, j = 15; i < 16; i++, j--) {
		addr6.s6_addr[i] = IP2Location_read8(handle, position + j);
	}

	return addr6;
}

// Alias to IP2Location_readIPv6Address()
IP2LOCATION_STATIC struct in6_addr IP2Location_read_ipv6_address(FILE *handle, uint32_t position)
{
	return IP2Location_readIPv6Address(handle, position);
}

IP2LOCATION_STATIC uint32_t IP2Location_read32(FILE *handle, uint32_t position)
{
	uint8_t byte1 = 0;
	uint8_t byte2 = 0;
	uint8_t byte3 = 0;
	uint8_t byte4 = 0;
	uint8_t *cache_shm = memory_pointer;
	size_t temp;

	// Read from file
	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position - 1, SEEK_SET);
		temp = fread(&byte1, 1, 1, handle);

		if (temp == 0) {
			return 0;
		}

		temp = fread(&byte2, 1, 1, handle);

		if (temp == 0) {
			return 0;
		}

		temp = fread(&byte3, 1, 1, handle);

		if (temp == 0) {
			return 0;
		}

		temp = fread(&byte4, 1, 1, handle);

		if (temp == 0) {
			return 0;
		}
	} else {
		byte1 = cache_shm[position - 1];
		byte2 = cache_shm[position];
		byte3 = cache_shm[position + 1];
		byte4 = cache_shm[position + 2];
	}

	return ((byte4 << 24) | (byte3 << 16) | (byte2 << 8) | (byte1));
}

IP2LOCATION_STATIC uint32_t IP2Location_read32_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset)
{
    uint8_t* addr;

    if (lookup_mode == IP2LOCATION_FILE_IO) {
        addr = buffer + position;
    } else {
        addr = memory_pointer + mem_offset + position - 1;
    }

    return ((uint32_t)addr[3] << 24) | ((uint32_t)addr[2] << 16) | ((uint32_t)addr[1] << 8) | ((uint32_t)addr[0]);
}

IP2LOCATION_STATIC uint8_t IP2Location_read8(FILE *handle, uint32_t position)
{
	uint8_t ret = 0;
	uint8_t *cache_shm = memory_pointer;
	size_t temp;

	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position - 1, SEEK_SET);
		temp = fread(&ret, 1, 1, handle);

		if (temp == 0) {
			return 0;
		}
	} else {
		ret = cache_shm[position - 1];
	}

	return ret;
}

IP2LOCATION_STATIC uint8_t IP2Location_read8_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset)
{
	uint8_t *cache_shm = memory_pointer;

	if (lookup_mode == IP2LOCATION_FILE_IO) {
		return buffer[position];
	} else {
		return cache_shm[mem_offset + position - 1];
	}
}

IP2LOCATION_STATIC char *IP2Location_readStr(FILE *handle, uint32_t position)
{
	uint8_t size = 0;
	char *str = 0;
	uint8_t *cache_shm = memory_pointer;
	size_t temp;

	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position, 0);
		temp = fread(&size, 1, 1, handle);

		if (temp == 0) {
			return strdup("");
		}

		str = (char *)malloc(size+1);
		memset(str, 0, size+1);

		temp = fread(str, size, 1, handle);

		if (temp == 0) {
			free(str);
			return strdup("");
		}
	} else {
		size = cache_shm[position];
		str = (char *)malloc(size + 1);
		memset(str, 0, size + 1);
		memcpy((void*) str, (void*)&cache_shm[position + 1], size);
	}

	return str;
}

// Alias to IP2Location_readStr()
// char *IP2Location_read_string(FILE *handle, uint32_t position)
// {
	// return IP2Location_readStr(handle, position);
// }

IP2LOCATION_STATIC char *IP2Location_read_string(FILE *handle, uint32_t position)
{
	uint8_t data[255];
	uint8_t size = 0;
	char* str = 0;
	uint8_t *cache_shm = memory_pointer;

	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position, 0);
		fread(&data, 255, 1, handle); // max size of string field + 1 byte for length
		size = data[0];
		str = (char *)malloc(size+1);
		memcpy(str, ((uint8_t*)data) + 1, size);
		str[size] = '\0'; // add null terminator
	} else {
		size = cache_shm[position];
		str = (char *)malloc(size + 1);
		memset(str, 0, size + 1);
		memcpy((void*) str, (void*)&cache_shm[position + 1], size);
	}
	return str;
}

IP2LOCATION_STATIC float IP2Location_readFloat(FILE *handle, uint32_t position)
{
	float ret = 0.0;
	uint8_t *cache_shm = memory_pointer;
	size_t temp;

#ifdef WORDS_BIGENDIAN
	char *p = (char *) &ret;

	// have to reverse the byte order
	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position - 1, 0);

		temp = fread(p + 3, 1, 1, handle);

		if (temp == 0) {
			return 0.0;
		}

		temp = fread(p + 2, 1, 1, handle);

		if (temp == 0) {
			return 0.0;
		}

		temp = fread(p + 1, 1, 1, handle);

		if (temp == 0) {
			return 0.0;
		}

		temp = fread(p, 1, 1, handle);

		if (temp == 0) {
			return 0.0;
		}
	} else {
		*(p+3) = cache_shm[position - 1];
		*(p+2) = cache_shm[position];
		*(p+1) = cache_shm[position + 1];
		*(p) = cache_shm[position + 2];
	}
#else
	if (lookup_mode == IP2LOCATION_FILE_IO && handle != NULL) {
		fseek(handle, position - 1, 0);
		temp = fread(&ret, 4, 1, handle);

		if (temp == 0) {
			return 0.0;
		}
	} else {
		memcpy((void*) &ret, (void*)&cache_shm[position - 1], 4);
	}
#endif
	return ret;
}

// Alias to IP2Location_readFloat()
IP2LOCATION_STATIC float IP2Location_read_float(FILE *handle, uint32_t position)
{
	return IP2Location_readFloat(handle, position);
}

IP2LOCATION_STATIC float IP2Location_read_float_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset)
{
	float ret = 0.0;
	uint8_t stuff[4];
	uint8_t *cache_shm = memory_pointer;

#ifdef WORDS_BIGENDIAN
	char *p = (char *) &ret;

	// have to reverse the byte order
	if (lookup_mode == IP2LOCATION_FILE_IO) {
		uint8_t temp[4];
		memcpy(&temp, buffer + position, 4);
		stuff[0] = temp[3];
		stuff[1] = temp[2];
		stuff[2] = temp[1];
		stuff[3] = temp[0];
		memcpy(&ret, &stuff, 4);
	} else {
		*(p+3) = cache_shm[mem_offset + position - 1];
		*(p+2) = cache_shm[mem_offset + position];
		*(p+1) = cache_shm[mem_offset + position + 1];
		*(p) = cache_shm[mem_offset + position + 2];
	}
#else
	if (lookup_mode == IP2LOCATION_FILE_IO) {
		memcpy(&stuff, buffer + position, 4);
		memcpy(&ret, &stuff, 4);
	} else {
		memcpy((void*) &ret, (void*)&cache_shm[mem_offset + position - 1], 4);
	}
#endif
	return ret;
}
