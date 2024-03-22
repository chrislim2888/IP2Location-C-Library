/*
 * IP2Location C library is distributed under MIT license
 * Copyright (c) 2013-2024 IP2Location.com. support at ip2location dot com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the MIT license
 */

#ifndef HAVE_IP2LOCATION_H
#define HAVE_IP2LOCATION_H

/*
 * Following define converts most of library functions into static ones
 * and prevents exporing defines without IP2LOCATION_ prefix.
#define IP2LOCATION_HIDDEN_INTERNALS
*/

#ifdef IP2LOCATION_HIDDEN_INTERNALS
#define IP2LOCATION_STATIC static
#else
#define IP2LOCATION_STATIC
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#if !defined(__APPLE__)
#include <stdlib.h>
#endif

#ifdef WIN32
#define int16_t short
#define int32_t int
#define int64_t long long int
#endif

#ifndef WIN32
#include <stdint.h>
#else

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t
#define uint16_t short
#endif

#ifndef int32_t
#define int32_t int
#endif

#ifndef int64_t
#define int64_t long long int
#endif

#ifndef uint32_t
#ifndef WIN32
#define uint32_t int
#else
#define uint32_t unsigned int
#endif
#endif
#endif

#define STR_HELPER(x) #x
#define MACROSTR(x) STR_HELPER(x)

#define IP2LOCATION_API_VERSION_MAJOR	8
#define IP2LOCATION_API_VERSION_MINOR	7
#define IP2LOCATION_API_VERSION_RELEASE	0
#define IP2LOCATION_API_VERSION		MACROSTR(IP2LOCATION_API_VERSION_MAJOR) "." MACROSTR(IP2LOCATION_API_VERSION_MINOR) "." MACROSTR(IP2LOCATION_API_VERSION_RELEASE)
#define IP2LOCATION_API_VERSION_NUMERIC (((IP2LOCATION_API_VERSION_MAJOR * 100) + IP2LOCATION_API_VERSION_MINOR) * 100 + IP2LOCATION_API_VERSION_RELEASE)

#define IP2LOCATION_MAX_IPV4_RANGE	4294967295U
#define IP2LOCATION_MAX_IPV6_RANGE	"340282366920938463463374607431768211455"
#define IP2LOCATION_IPV4	0
#define IP2LOCATION_IPV6	1

/* Attribute bitfield */
enum DB_FIELDS {
	IP2LOCATION_COUNTRYSHORT	=	0,
	IP2LOCATION_COUNTRYLONG,
	IP2LOCATION_REGION,
	IP2LOCATION_CITY,
	IP2LOCATION_ISP,
	IP2LOCATION_LATITUDE,
	IP2LOCATION_LONGITUDE,
	IP2LOCATION_DOMAINNAME, /* 7 */
	IP2LOCATION_ZIPCODE,
	IP2LOCATION_TIMEZONE,
	IP2LOCATION_NETSPEED,
	IP2LOCATION_IDDCODE,
	IP2LOCATION_AREACODE,
	IP2LOCATION_WEATHERSTATIONCODE,
	IP2LOCATION_WEATHERSTATIONNAME,
	IP2LOCATION_MCC, /* 15 */
	IP2LOCATION_MNC,
	IP2LOCATION_MOBILEBRAND,
	IP2LOCATION_ELEVATION,
	IP2LOCATION_USAGETYPE,
	IP2LOCATION_ADDRESSTYPE,
	IP2LOCATION_CATEGORY,
	IP2LOCATION_DISTRICT,
	IP2LOCATION_ASN, /* 23 */
	IP2LOCATION_AS,
/*
	SPARE
	SPARE
	SPARE
	SPARE
	SPARE
	SPARE
	SPARE // 31
*/
};

#define IP2LOCATION_ALL					0xffffffff

#define IP2LOCATION_DEFAULT				0x0001
#define IP2LOCATION_NO_EMPTY_STRING		0x0002
#define IP2LOCATION_NO_LEADING			0x0004
#define IP2LOCATION_NO_TRAILING			0x0008


#define IP2LOCATION_INVALID_IP_ADDRESS "INVALID IP ADDRESS"
#define IP2LOCATION_IPV6_ADDRESS_MISSING_IN_IPV4_BIN "IPV6 ADDRESS MISSING IN IPV4 BIN"
#define IP2LOCATION_NOT_SUPPORTED "This parameter is unavailable for selected data file. Please upgrade the data file."
#define IP2LOCATION_INVALID_BIN_DATABASE "Incorrect IP2Location BIN file format. Please make sure that you are using the latest IP2Location BIN file."
#define IP2LOCATION_SHM "IP2location_Shm"
#define IP2LOCATION_MAP_ADDR 4194500608

enum IP2Location_lookup_mode {
	IP2LOCATION_FILE_IO,
	IP2LOCATION_CACHE_MEMORY,
	IP2LOCATION_SHARED_MEMORY,
};

struct _IP2Location
{
	FILE *file;
	uint8_t database_type;
	uint8_t database_column;
	uint8_t database_day;
	uint8_t database_month;
	uint8_t database_year;
	uint8_t product_code;
	uint8_t license_code;
	uint8_t dummy; /* 32-bit alignment */
	uint32_t database_count;
	uint32_t database_address;
	uint32_t ip_version;
	uint32_t ipv4_database_count;
	uint32_t ipv4_database_address;
	uint32_t ipv6_database_count;
	uint32_t ipv6_database_address;
	uint32_t ipv4_index_base_address;
	uint32_t ipv6_index_base_address;
	uint32_t database_size;
}
;

typedef struct _IP2Location IP2Location;

typedef struct {
	char *country_short;
	char *country_long;
	char *region;
	char *city;
	char *isp;
	char *domain;
	union {
		char *zipcode;
		char *zip_code;
	};
	union {
		char *timezone;
		char *time_zone;
	};
	union {
		char *netspeed;
		char *net_speed;
	};
	union {
		char *iddcode;
		char *idd_code;
	};
	union {
		char *areacode;
		char *area_code;
	};
	union {
		char *weatherstationcode;
		char *weather_station_code;
	};
	union {
		char *weatherstationname;
		char *weather_station_name;
	};
	char *mcc;
	char *mnc;
	union {
		char *mobilebrand;
		char *mobile_brand;
	};
	union {
		char *usagetype;
		char *usage_type;
	};

	float latitude;
	float longitude;
	float elevation;

	char *address_type;
	char *category;
	char *district;
	char *asn;
	char *as;
} IP2LocationRecord;

/* Public functions */
IP2Location *IP2Location_open(char *bin);
int IP2Location_open_mem(IP2Location *handler, enum IP2Location_lookup_mode);
int IP2Location_set_lookup_mode(IP2Location *handler, enum IP2Location_lookup_mode);
uint32_t IP2Location_close(IP2Location *handler);

void IP2Location_delete_shm();
void IP2Location_clear_memory();
void IP2Location_delete_shared_memory();

IP2LocationRecord *IP2Location_get_country_short(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_country_long(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_region(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_city (IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_isp(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_latitude(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_longitude(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_domain(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_timezone(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_iddcode(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_areacode(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_weatherstationcode(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_weatherstationname(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_mcc(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_mnc(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_mobilebrand(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_elevation(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_usagetype(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_addresstype(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_category(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_district(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_asn(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_as(IP2Location *handler, char *ip);
IP2LocationRecord *IP2Location_get_all(IP2Location *handler, char *ip);
void IP2Location_free_record(IP2LocationRecord *record);

unsigned long int IP2Location_api_version_num(void);
char *IP2Location_api_version_string(void);
char *IP2Location_lib_version_string(void);
char *IP2Location_bin_version(IP2Location *handler);
unsigned long int IP2Location_api_version_number(void);

#ifndef IP2LOCATION_HIDDEN_INTERNALS
#define API_VERSION IP2LOCATION_API_VERSION
#define API_VERSION_MAJOR IP2LOCATION_API_VERSION_MAJOR
#define API_VERSION_MINOR IP2LOCATION_API_VERSION_MINOR
#define API_VERSION_RELEASE IP2LOCATION_API_VERSION_RELEASE
#define API_VERSION_NUMERIC IP2LOCATION_API_VERSION_NUMERIC
#define MAX_IPV4_RANGE IP2LOCATION_MAX_IPV4_RANGE
#define MAX_IPV6_RANGE IP2LOCATION_MAX_IPV6_RANGE
#define IPV4 IP2LOCATION_IPV4
#define IPV6 IP2LOCATION_IPV6
#define COUNTRYSHORT IP2LOCATION_COUNTRYSHORT
#define COUNTRYLONG IP2LOCATION_COUNTRYLONG
#define REGION IP2LOCATION_REGION
#define CITY IP2LOCATION_CITY
#define ISP IP2LOCATION_ISP
#define LATITUDE IP2LOCATION_LATITUDE
#define LONGITUDE IP2LOCATION_LONGITUDE
#define DOMAINNAME IP2LOCATION_DOMAINNAME
#define ZIPCODE IP2LOCATION_ZIPCODE
#define TIMEZONE IP2LOCATION_TIMEZONE
#define NETSPEED IP2LOCATION_NETSPEED
#define IDDCODE IP2LOCATION_IDDCODE
#define AREACODE IP2LOCATION_AREACODE
#define WEATHERSTATIONCODE IP2LOCATION_WEATHERSTATIONCODE
#define WEATHERSTATIONNAME IP2LOCATION_WEATHERSTATIONNAME
#define MCC IP2LOCATION_MCC
#define MNC IP2LOCATION_MNC
#define MOBILEBRAND IP2LOCATION_MOBILEBRAND
#define ELEVATION IP2LOCATION_ELEVATION
#define USAGETYPE IP2LOCATION_USAGETYPE
#define ADDRESSTYPE IP2LOCATION_ADDRESSTYPE
#define CATEGORY IP2LOCATION_CATEGORY
#define DISTRICT IP2LOCATION_DISTRICT
#define ASN IP2LOCATION_ASN
#define AS IP2LOCATION_AS
#define ALL IPLOCATION_ALL
#define DEFAULT IP2LOCATION_DEFAULT
#define NO_EMPTY_STRING IP2LOCATION_NO_EMPTY_STRING
#define NO_LEADING IP2LOCATION_NO_LEADING
#define NO_TRAILING IP2LOCATION_NO_TRAILING
#define INVALID_IP_ADDRESS IP2LOCATION_INVALID_IP_ADDRESS
#define IPV6_ADDRESS_MISSING_IN_IPV4_BIN IP2LOCATION_IPV6_ADDRESS_MISSING_IN_IPV4_BIN
#define NOT_SUPPORTED IP2LOCATION_NOT_SUPPORTED
#define INVALID_BIN_DATABASE IP2LOCATION_INVALID_BIN_DATABASE
#define MAP_ADDR IP2LOCATION_MAP_ADDR

struct in6_addr IP2Location_readIPv6Address(FILE *handle, uint32_t position);
uint32_t IP2Location_read32(FILE *handle, uint32_t position);
uint32_t IP2Location_read32_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
uint8_t IP2Location_read8(FILE *handle, uint32_t position);
uint8_t IP2Location_read8_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
char *IP2Location_readStr(FILE *handle, uint32_t position);
float IP2Location_readFloat(FILE *handle, uint32_t position);
int32_t IP2Location_DB_set_file_io();
int32_t IP2Location_DB_set_memory_cache(FILE *file);
int32_t IP2Location_DB_set_shared_memory(FILE *file);
int32_t IP2Location_DB_close(FILE *file);
void IP2Location_DB_del_shm();

int32_t IP2Location_close_memory(FILE* file);
float IP2Location_read_float(FILE* handle, uint32_t position);
float IP2Location_read_float_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
struct in6_addr IP2Location_read_ipv6_address(FILE* handle, uint32_t position);
struct in6_addr IP2Location_read128_row(uint8_t* buffer, uint32_t position, uint32_t mem_offset);
char *IP2Location_read_string(FILE* handle, uint32_t position);
int32_t IP2Location_set_memory_cache(FILE* file);
int32_t IP2Location_set_shared_memory(FILE* file);
#endif /* IP2LOCATION_HIDDEN_INTERNALS */

#ifdef __cplusplus
}
#endif
#endif
