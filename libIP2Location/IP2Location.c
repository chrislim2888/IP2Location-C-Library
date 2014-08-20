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
#include <ws2tcpip.h>
#else
#include <stdint.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include <stdio.h>


#include "IP2Location.h"
#include "IP2Loc_DBInterface.h"

uint8_t COUNTRY_POSITION[25]             = {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
uint8_t REGION_POSITION[25]              = {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
uint8_t CITY_POSITION[25]                = {0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
uint8_t ISP_POSITION[25]                 = {0, 0, 3, 0, 5, 0, 7, 5, 7, 0, 8, 0, 9, 0, 9, 0, 9, 0, 9, 7, 9, 0, 9, 7, 9};
uint8_t LATITUDE_POSITION[25]            = {0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
uint8_t LONGITUDE_POSITION[25]           = {0, 0, 0, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
uint8_t DOMAIN_POSITION[25]              = {0, 0, 0, 0, 0, 0, 0, 6, 8, 0, 9, 0, 10,0, 10, 0, 10, 0, 10, 8, 10, 0, 10, 8, 10};
uint8_t ZIPCODE_POSITION[25]             = {0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 0, 7, 7, 7, 0, 7, 0, 7, 7, 7, 0, 7};
uint8_t TIMEZONE_POSITION[25]            = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 7, 8, 8, 8, 7, 8, 0, 8, 8, 8, 0, 8};
uint8_t NETSPEED_POSITION[25]            = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 11,0, 11,8, 11, 0, 11, 0, 11, 0, 11};
uint8_t IDDCODE_POSITION[25]             = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 12, 0, 12, 0, 12, 9, 12, 0, 12};
uint8_t AREACODE_POSITION[25]            = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10 ,13 ,0, 13, 0, 13, 10, 13, 0, 13};
uint8_t WEATHERSTATIONCODE_POSITION[25]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 14, 0, 14, 0, 14, 0, 14};
uint8_t WEATHERSTATIONNAME_POSITION[25]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 15, 0, 15, 0, 15, 0, 15};
uint8_t MCC_POSITION[25]                 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 16, 0, 16, 9, 16};
uint8_t MNC_POSITION[25]                 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,17, 0, 17, 10, 17};
uint8_t MOBILEBRAND_POSITION[25]         = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11,18, 0, 18, 11, 18};
uint8_t ELEVATION_POSITION[25]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 19, 0, 19};
uint8_t USAGETYPE_POSITION[25]           = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 20};

static int IP2Location_initialize(IP2Location *loc);
static IP2LocationRecord *IP2Location_new_record();
static uint32_t IP2Location_ip2no(char* ip);
static struct in6_addr_local  IP2Location_ipv6_to_no(char* ipaddr);
static int IP2Location_ip_is_ipv4 (char* ipaddr);
static int IP2Location_ip_is_ipv6 (char* ipaddr);
static IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ip, uint32_t mode);
static IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *loc, char *ipstring, uint32_t mode);

static int32_t openMemFlag = 0;
// Description: Open the IP2Location database file
IP2Location *IP2Location_open(char *db)
{
	FILE *f;
	IP2Location *loc;

	if ( ( f = fopen( db, "rb")) == NULL) {
		printf("IP2Location library error in opening database %s.\n", db);
		return NULL;
	}

	loc = (IP2Location *) calloc(1, sizeof(IP2Location));
	loc->filehandle = f;

	IP2Location_initialize(loc);
	return loc;
}
//Description: This function to set the DB access type.
int32_t IP2Location_open_mem(IP2Location *loc, enum IP2Location_mem_type mtype)
{
	if( loc == NULL)
		return -1;

	/*Once IP2Location_open_mem is called, it can not be called again till
	 * IP2Location_close is called*/ 
	if(openMemFlag != 0)
		return -1;
	openMemFlag = 1;

	if( mtype == IP2LOCATION_FILE_IO ) {
		return 0; //Just return, by default its IP2LOCATION_FILE_IO
	}else if(  mtype == IP2LOCATION_CACHE_MEMORY ) { 
		return IP2Location_DB_set_memory_cache(loc->filehandle);
	}else if ( mtype == IP2LOCATION_SHARED_MEMORY ) {
		return IP2Location_DB_set_shared_memory(loc->filehandle);
	}else
		return -1;
}

// Description: Close the IP2Location database file
uint32_t IP2Location_close(IP2Location *loc)
{
	openMemFlag = 0;
	if (loc != NULL) {
		IP2Location_DB_close(loc->filehandle);
		free(loc);
	}
	return 0;
}

// Description: Delete IP2Location shared memory if its present.  
void IP2Location_delete_shm()
{
	IP2Location_DB_del_shm();
}

// Description: Startup
static int IP2Location_initialize(IP2Location *loc)
{
	loc->databasetype   = IP2Location_read8(loc->filehandle, 1);
	loc->databasecolumn = IP2Location_read8(loc->filehandle, 2);
	loc->databaseyear    = IP2Location_read8(loc->filehandle, 3);
	loc->databasemonth  = IP2Location_read8(loc->filehandle, 4);
	loc->databaseday   = IP2Location_read8(loc->filehandle, 5);

	loc->databasecount  = IP2Location_read32(loc->filehandle, 6);
	loc->databaseaddr   = IP2Location_read32(loc->filehandle, 10);
	loc->ipversion      = IP2Location_read32(loc->filehandle, 14);

	loc->ipv4databasecount  = IP2Location_read32(loc->filehandle, 6);
	loc->ipv4databaseaddr   = IP2Location_read32(loc->filehandle, 10);
	loc->ipv6databasecount  = IP2Location_read32(loc->filehandle, 14);
	loc->ipv6databaseaddr   = IP2Location_read32(loc->filehandle, 18);

	return 0;
}

// Description: Compare to ipv6 address
int ipv6_compare(struct in6_addr_local *addr1, struct in6_addr_local *addr2)
{
	int i, ret = 0;
	for(i = 0 ; i < 16 ; i++ )
	{
		if(addr1->u.addr8[i] > addr2->u.addr8[i])
		{
			ret = 1;
			break;
		}
		else if(addr1->u.addr8[i] < addr2->u.addr8[i])
		{
			ret = -1;
			break;
		}
	}
	return ret;
}

typedef struct ipv_t {
    	uint32_t ipversion;
    	uint32_t ipv4;
	struct in6_addr_local ipv6;
} ipv_t;

// Parses IPv[46] addresses and returns both the version of address
// and binary address used for searching
// You can implement domain name lookup here as well
// ipversion will be -1 on error (or something other than 4 or 6)
static ipv_t parse_addr(const char *addr)
{
	ipv_t parsed;
    	if (IP2Location_ip_is_ipv6((char *)addr))
    	{
		// Convert ::FFFF:x.y.z.w to IPv4
		if (strlen(addr) > 7 &&
			addr[0] == ':' &&
			addr[1] == ':' &&
			(addr[2] == 'F' || addr[2] == 'f') &&
			(addr[3] == 'F' || addr[3] == 'f') &&
			(addr[4] == 'F' || addr[4] == 'f') &&
			(addr[5] == 'F' || addr[5] == 'f') &&
			addr[6] == ':' && IP2Location_ip_is_ipv4((char *)addr + 7))
		{
			parsed.ipversion = 4;	
			parsed.ipv4 = IP2Location_ip2no((char *)addr + 7);
		}
		else
		{
			parsed.ipversion = 6;	
   	     		inet_pton(AF_INET6, addr, &parsed.ipv6);
		}

    	}
    	else if (IP2Location_ip_is_ipv4((char *)addr))
    	{
		parsed.ipversion = 4;	
		parsed.ipv4 = IP2Location_ip2no((char *)addr);
    	}
    	else
    	{
		parsed.ipversion = -1;	
    	}
    	return parsed;
}

// Description: Get country code
IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, COUNTRYSHORT);
}

// Description: Get country name
IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, COUNTRYLONG);
}

// Description: Get the name of state/region
IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, REGION);
}

// Description: Get city name
IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, CITY);
}

// Description: Get ISP name
IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ISP);
}

// Description: Get latitude
IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, LATITUDE);
}

// Description: Get longitude
IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, LONGITUDE);
}

// Description: Get domain name
IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, DOMAIN);
}

// Description: Get ZIP code
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ZIPCODE);
}

// Description: Get time zone
IP2LocationRecord *IP2Location_get_timezone(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, TIMEZONE);
}

// Description: Get net speed
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, NETSPEED);
}

// Description: Get IDD code
IP2LocationRecord *IP2Location_get_iddcode(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, IDDCODE);
}

// Description: Get area code
IP2LocationRecord *IP2Location_get_areacode(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, AREACODE);
}

// Description: Get weather station code
IP2LocationRecord *IP2Location_get_weatherstationcode(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, WEATHERSTATIONCODE);
}

// Description: Get weather station name
IP2LocationRecord *IP2Location_get_weatherstationname(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, WEATHERSTATIONNAME);
}

// Description: Get mobile country code
IP2LocationRecord *IP2Location_get_mcc(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, MCC);
}

// Description: Get mobile national code
IP2LocationRecord *IP2Location_get_mnc(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, MNC);
}

// Description: Get mobile carrier brand
IP2LocationRecord *IP2Location_get_mobilebrand(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, MOBILEBRAND);
}

// Description: Get elevation
IP2LocationRecord *IP2Location_get_elevation(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ELEVATION);
}

// Description: Get usage type
IP2LocationRecord *IP2Location_get_usagetype(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, USAGETYPE);
}

// Description: Get all records of a IPv6
IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip)
{
	return IP2Location_get_record(loc, ip, ALL);
}

static IP2LocationRecord *bad_record(const char *message)
{
	IP2LocationRecord *record = IP2Location_new_record();
	record->country_short = strdup(message);
	record->country_long = strdup(message);
	record->region = strdup(message);
	record->city = strdup(message);
	record->isp = strdup(message);
	record->latitude = 0;
	record->longitude = 0;
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
	record->elevation = 0;
	record->usagetype = strdup(message);
	return record;
}

static IP2LocationRecord *read_record(IP2Location *loc, uint32_t rowaddr, uint32_t mode)
{
	uint8_t dbtype = loc->databasetype;
	FILE *handle = loc->filehandle;
	IP2LocationRecord *record = IP2Location_new_record();

	if ((mode & COUNTRYSHORT) && (COUNTRY_POSITION[dbtype] != 0)) {
		record->country_short = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (COUNTRY_POSITION[dbtype]-1)));
	} else {
		record->country_short = strdup(NOT_SUPPORTED);
	}
	
	if ((mode & COUNTRYLONG) && (COUNTRY_POSITION[dbtype] != 0)) {
		record->country_long = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
	} else {
		record->country_long = strdup(NOT_SUPPORTED);
	}

	if ((mode & REGION) && (REGION_POSITION[dbtype] != 0)) {
		record->region = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (REGION_POSITION[dbtype]-1)));
	} else {
		record->region = strdup(NOT_SUPPORTED);
	}

	if ((mode & CITY) && (CITY_POSITION[dbtype] != 0)) {
		record->city = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (CITY_POSITION[dbtype]-1)));
	} else {
		record->city = strdup(NOT_SUPPORTED);
	}

	if ((mode & ISP) && (ISP_POSITION[dbtype] != 0)) {
		record->isp = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (ISP_POSITION[dbtype]-1)));
	} else {
		record->isp = strdup(NOT_SUPPORTED);
	}

	if ((mode & LATITUDE) && (LATITUDE_POSITION[dbtype] != 0)) {
		record->latitude = IP2Location_readFloat(handle, rowaddr + 4 * (LATITUDE_POSITION[dbtype]-1));
	} else {
		record->latitude = 0.0;
	}

	if ((mode & LONGITUDE) && (LONGITUDE_POSITION[dbtype] != 0)) {
		record->longitude = IP2Location_readFloat(handle, rowaddr + 4 * (LONGITUDE_POSITION[dbtype]-1));
	} else {
		record->longitude = 0.0;
	}

	if ((mode & DOMAIN) && (DOMAIN_POSITION[dbtype] != 0)) {
		record->domain = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (DOMAIN_POSITION[dbtype]-1)));
	} else {
		record->domain = strdup(NOT_SUPPORTED);
	}

	if ((mode & ZIPCODE) && (ZIPCODE_POSITION[dbtype] != 0)) {
		record->zipcode = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (ZIPCODE_POSITION[dbtype]-1)));
	} else {
		record->zipcode = strdup(NOT_SUPPORTED);
	}
	
	if ((mode & TIMEZONE) && (TIMEZONE_POSITION[dbtype] != 0)) {
		record->timezone = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (TIMEZONE_POSITION[dbtype]-1)));
	} else {
		record->timezone = strdup(NOT_SUPPORTED);
	}

	if ((mode & NETSPEED) && (NETSPEED_POSITION[dbtype] != 0)) {
		record->netspeed = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (NETSPEED_POSITION[dbtype]-1)));
	} else {
		record->netspeed = strdup(NOT_SUPPORTED);
	}

	if ((mode & IDDCODE) && (IDDCODE_POSITION[dbtype] != 0)) {
		record->iddcode = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (IDDCODE_POSITION[dbtype]-1)));
	} else {
		record->iddcode = strdup(NOT_SUPPORTED);
	}

	if ((mode & AREACODE) && (AREACODE_POSITION[dbtype] != 0)) {
		record->areacode = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (AREACODE_POSITION[dbtype]-1)));
	} else {
		record->areacode = strdup(NOT_SUPPORTED);
	}

	if ((mode & WEATHERSTATIONCODE) && (WEATHERSTATIONCODE_POSITION[dbtype] != 0)) {
		record->weatherstationcode = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (WEATHERSTATIONCODE_POSITION[dbtype]-1)));
	} else {
		record->weatherstationcode = strdup(NOT_SUPPORTED);
	}

	if ((mode & WEATHERSTATIONNAME) && (WEATHERSTATIONNAME_POSITION[dbtype] != 0)) {
		record->weatherstationname = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (WEATHERSTATIONNAME_POSITION[dbtype]-1)));
	} else {
		record->weatherstationname = strdup(NOT_SUPPORTED);
	}

	if ((mode & MCC) && (MCC_POSITION[dbtype] != 0)) {
		record->mcc = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (MCC_POSITION[dbtype]-1)));
	} else {
		record->mcc = strdup(NOT_SUPPORTED);
	}

	if ((mode & MNC) && (MNC_POSITION[dbtype] != 0)) {
		record->mnc = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (MNC_POSITION[dbtype]-1)));
	} else {
		record->mnc = strdup(NOT_SUPPORTED);
	}

	if ((mode & MOBILEBRAND) && (MOBILEBRAND_POSITION[dbtype] != 0)) {
		record->mobilebrand = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (MOBILEBRAND_POSITION[dbtype]-1)));
	} else {
		record->mobilebrand = strdup(NOT_SUPPORTED);
	}

	if ((mode & ELEVATION) && (ELEVATION_POSITION[dbtype] != 0)) {
		record->elevation = atof(IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (ELEVATION_POSITION[dbtype]-1))));
	} else {
		record->elevation = 0.0;
	}
	
	if ((mode & USAGETYPE) && (USAGETYPE_POSITION[dbtype] != 0)) {
		record->usagetype = IP2Location_readStr(handle, IP2Location_read32(handle, rowaddr + 4 * (USAGETYPE_POSITION[dbtype]-1)));
	} else {
		record->usagetype = strdup(NOT_SUPPORTED);
	}
	return record;
}

// Description: Get record for a IPv6 from database
static IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->ipv6databaseaddr;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = loc->ipv6databasecount;
	uint32_t mid = 0;

	struct in6_addr_local ipfrom;
	struct in6_addr_local ipto;
	struct in6_addr_local ipno;

	ipv_t parsed_ipv = parse_addr(ipstring);
	if (parsed_ipv.ipversion == 4) {
		return IP2Location_get_record(loc, ipstring, mode);
	}
	
	if (parsed_ipv.ipversion != 6) {
		return bad_record(INVALID_IPV6_ADDRESS);
	}

        ipno = parsed_ipv.ipv6;

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		ipfrom = IP2Location_readIPv6Address(handle, baseaddr + mid * (dbcolumn * 4 + 12)); 
		ipto = IP2Location_readIPv6Address(handle, baseaddr + ( mid + 1 ) * (dbcolumn * 4 + 12)); 
		
		if( (ipv6_compare(&ipno, &ipfrom) >= 0) && (ipv6_compare(&ipno, &ipto) < 0))
		{
			return read_record(loc, baseaddr + mid * (dbcolumn * 4 + 12) + 12, mode);
		}
		else
		{
			if ( ipv6_compare(&ipno, &ipfrom) < 0) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	return NULL;
}

static IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->ipv4databaseaddr;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = loc->ipv4databasecount;
	uint32_t mid = 0;

	uint32_t ipno;
	uint32_t ipfrom;
	uint32_t ipto;

	ipv_t parsed_ipv = parse_addr(ipstring);
	if (parsed_ipv.ipversion == 6) {
		return IP2Location_get_ipv6_record(loc, ipstring, mode);
	}
	
	if (parsed_ipv.ipversion != 4) {
		return bad_record(INVALID_IPV4_ADDRESS);
	}

	ipno = parsed_ipv.ipv4;
	if (ipno == (uint32_t) MAX_IPV4_RANGE) {
		ipno = ipno - 1;
	}

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		ipfrom = IP2Location_read32(handle, baseaddr + mid * dbcolumn * 4);
		ipto 	= IP2Location_read32(handle, baseaddr + (mid + 1) * dbcolumn * 4);

		if ((ipno >= ipfrom) && (ipno < ipto)) 
		{
			return read_record(loc, baseaddr + (mid * dbcolumn * 4), mode);
		}
		else
		{
			if ( ipno < ipfrom ) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	return NULL;
}


static IP2LocationRecord *IP2Location_new_record()
{
	IP2LocationRecord *record = (IP2LocationRecord *) calloc(1, sizeof(IP2LocationRecord));
	return record;
}


void IP2Location_free_record(IP2LocationRecord *record)
{
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
	free(record);
}

static uint32_t IP2Location_ip2no(char* ipstring)
{
	uint32_t ip = inet_addr(ipstring);
	uint8_t *ptr = (uint8_t *) &ip;
	uint32_t a = 0;
	
	if (ipstring != NULL) {
		a =  (uint8_t)(ptr[3]);
		a += (uint8_t)(ptr[2]) * 256;
		a += (uint8_t)(ptr[1]) * 256 * 256;
		a += (uint8_t)(ptr[0]) * 256 * 256 * 256;
	}
	return a;
}

static int IP2Location_ip_is_ipv4 (char* ipaddr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, ipaddr, &(sa.sin_addr));
}

static int IP2Location_ip_is_ipv6 (char* ipaddr) {
	struct in6_addr_local ipv6;
	return inet_pton(AF_INET6, ipaddr, &ipv6);
}
