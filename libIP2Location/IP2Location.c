

#ifdef WIN32
#include <winsock2.h>
#else
#include <stdint.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include <stdio.h>


#include "imath.h"
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

	loc = (IP2Location *) malloc(sizeof(IP2Location));
	memset(loc, 0, sizeof(IP2Location));

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

// Descrption: Startup
int IP2Location_initialize(IP2Location *loc)
{
	loc->databasetype   = IP2Location_read8(loc->filehandle, 1);
	loc->databasecolumn = IP2Location_read8(loc->filehandle, 2);
	loc->databaseyear    = IP2Location_read8(loc->filehandle, 3);
	loc->databasemonth  = IP2Location_read8(loc->filehandle, 4);
	loc->databaseday   = IP2Location_read8(loc->filehandle, 5);
	loc->databasecount  = IP2Location_read32(loc->filehandle, 6);
	loc->databaseaddr   = IP2Location_read32(loc->filehandle, 10);
	loc->ipversion      = IP2Location_read32(loc->filehandle, 14);
	return 0;
}

// Description: Get country code
IP2LocationRecord *IP2Location_get_country_short(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, COUNTRYSHORT);
	} else {
		return IP2Location_get_record(loc, ip, COUNTRYSHORT);
	}
}

// Description: Get country name
IP2LocationRecord *IP2Location_get_country_long(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, COUNTRYLONG);
	} else {
		return IP2Location_get_record(loc, ip, COUNTRYLONG);
	}
}

// Description: Get the name of state/region
IP2LocationRecord *IP2Location_get_region(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, REGION);
	} else {
		return IP2Location_get_record(loc, ip, REGION);
	}
}

// Description: Get city name
IP2LocationRecord *IP2Location_get_city (IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, CITY);
	} else {
		return IP2Location_get_record(loc, ip, CITY);
	}
}

// Description: Get ISP name
IP2LocationRecord *IP2Location_get_isp(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ISP);
	} else {
		return IP2Location_get_record(loc, ip, ISP);
	}
}

// Description: Get latitude
IP2LocationRecord *IP2Location_get_latitude(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, LATITUDE);
	} else {
		return IP2Location_get_record(loc, ip, LATITUDE);
	}
}

// Description: Get longitude
IP2LocationRecord *IP2Location_get_longitude(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, LONGITUDE);
	} else {
		return IP2Location_get_record(loc, ip, LONGITUDE);
	}
}

// Description: Get domain name
IP2LocationRecord *IP2Location_get_domain(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, DOMAIN);
	} else {
		return IP2Location_get_record(loc, ip, DOMAIN);
	}
}

// Description: Get ZIP code
IP2LocationRecord *IP2Location_get_zipcode(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ZIPCODE);
	} else {
		return IP2Location_get_record(loc, ip, ZIPCODE);
	}
}

// Description: Get time zone
IP2LocationRecord *IP2Location_get_timezone(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, TIMEZONE);
	} else {
		return IP2Location_get_record(loc, ip, TIMEZONE);
	}
}

// Description: Get net speed
IP2LocationRecord *IP2Location_get_netspeed(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, NETSPEED);
	} else {
		return IP2Location_get_record(loc, ip, NETSPEED);
	}
}

// Description: Get IDD code
IP2LocationRecord *IP2Location_get_iddcode(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, IDDCODE);
	} else {
		return IP2Location_get_record(loc, ip, IDDCODE);
	}
}

// Description: Get area code
IP2LocationRecord *IP2Location_get_areacode(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, AREACODE);
	} else {
		return IP2Location_get_record(loc, ip, AREACODE);
	}
}

// Description: Get weather station code
IP2LocationRecord *IP2Location_get_weatherstationcode(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, WEATHERSTATIONCODE);
	} else {
		return IP2Location_get_record(loc, ip, WEATHERSTATIONCODE);
	}
}

// Description: Get weather station name
IP2LocationRecord *IP2Location_get_weatherstationname(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, WEATHERSTATIONNAME);
	} else {
		return IP2Location_get_record(loc, ip, WEATHERSTATIONNAME);
	}
}

// Description: Get mobile country code
IP2LocationRecord *IP2Location_get_mcc(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, MCC);
	} else {
		return IP2Location_get_record(loc, ip, MCC);
	}
}

// Description: Get mobile national code
IP2LocationRecord *IP2Location_get_mnc(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, MNC);
	} else {
		return IP2Location_get_record(loc, ip, MNC);
	}
}

// Description: Get mobile carrier brand
IP2LocationRecord *IP2Location_get_mobilebrand(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, MOBILEBRAND);
	} else {
		return IP2Location_get_record(loc, ip, MOBILEBRAND);
	}
}

// Description: Get elevation
IP2LocationRecord *IP2Location_get_elevation(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ELEVATION);
	} else {
		return IP2Location_get_record(loc, ip, ELEVATION);
	}
}

// Description: Get usage type
IP2LocationRecord *IP2Location_get_usagetype(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, USAGETYPE);
	} else {
		return IP2Location_get_record(loc, ip, USAGETYPE);
	}
}

// Description: Get all records of a IPv6
IP2LocationRecord *IP2Location_get_all(IP2Location *loc, char *ip)
{
	if (loc->ipversion == IPV6) {
		return IP2Location_get_ipv6_record(loc, ip, ALL);
	} else {
		return IP2Location_get_record(loc, ip, ALL);
	}
}

// Description: Get record for a IPv6 from database
IP2LocationRecord *IP2Location_get_ipv6_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	uint8_t dbtype = loc->databasetype;
	mpz_t ipno;
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->databaseaddr;
	uint32_t dbcount = loc->databasecount;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = dbcount;
	uint32_t mid = 0;
	mpz_t ipfrom, ipto;
	IP2LocationRecord *record = IP2Location_new_record();
	mp_int_init(&ipfrom);
	mp_int_init(&ipto);
		
	
	if(IP2Location_ip_is_ipv6(ipstring) == 0 ) {
		record->country_short = strdup(INVALID_IPV6_ADDRESS);
		record->country_long = strdup(INVALID_IPV6_ADDRESS);
		record->region = strdup(INVALID_IPV6_ADDRESS);
		record->city = strdup(INVALID_IPV6_ADDRESS);
		record->isp = strdup(INVALID_IPV6_ADDRESS);
		record->latitude = 0;
		record->longitude = 0;
		record->domain = strdup(INVALID_IPV6_ADDRESS);
		record->zipcode = strdup(INVALID_IPV6_ADDRESS);
		record->timezone = strdup(INVALID_IPV6_ADDRESS);
		record->netspeed = strdup(INVALID_IPV6_ADDRESS);
		record->iddcode = strdup(INVALID_IPV6_ADDRESS);
		record->areacode = strdup(INVALID_IPV6_ADDRESS);
		record->weatherstationcode = strdup(INVALID_IPV6_ADDRESS);
		record->weatherstationname = strdup(INVALID_IPV6_ADDRESS);
		record->mcc = strdup(INVALID_IPV6_ADDRESS);
		record->mnc = strdup(INVALID_IPV6_ADDRESS);
		record->mobilebrand = strdup(INVALID_IPV6_ADDRESS);
		record->elevation = 0;
		record->usagetype = strdup(INVALID_IPV6_ADDRESS);

		return record;
	}

	ipno = IP2Location_ipv6_to_no(ipstring);

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		mp_int_read_string(&ipfrom, 10, IP2Location_read128(handle, baseaddr + mid * (dbcolumn * 4 + 12))); 
		mp_int_read_string(&ipto, 10, IP2Location_read128(handle, baseaddr + (mid + 1) * (dbcolumn * 4 + 12)));
		
		if( (mp_int_compare(&ipno, &ipfrom) >= 0) && (mp_int_compare(&ipno, &ipto) < 0))
		{
			if ((mode & COUNTRYSHORT) && (COUNTRY_POSITION[dbtype] != 0)) {
				/* $ip, $baseaddr + $mid *($dbcolumn * 4 + 12) + 12 + 4 * */
				record->country_short = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (COUNTRY_POSITION[dbtype]-1)));
			} else {
				record->country_short = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & COUNTRYLONG) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_long = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
			} else {
				record->country_long = strdup(NOT_SUPPORTED);
			}

			if ((mode & REGION) && (REGION_POSITION[dbtype] != 0)) {
				record->region = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (REGION_POSITION[dbtype]-1)));
			} else {
				record->region = strdup(NOT_SUPPORTED);
			}

			if ((mode & CITY) && (CITY_POSITION[dbtype] != 0)) {
				record->city = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (CITY_POSITION[dbtype]-1)));
			} else {
				record->city = strdup(NOT_SUPPORTED);
			}

			if ((mode & ISP) && (ISP_POSITION[dbtype] != 0)) {
				record->isp = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (ISP_POSITION[dbtype]-1)));
			} else {
				record->isp = strdup(NOT_SUPPORTED);
			}

			if ((mode & LATITUDE) && (LATITUDE_POSITION[dbtype] != 0)) {
				record->latitude = IP2Location_readFloat(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (LATITUDE_POSITION[dbtype]-1));
			} else {
				record->latitude = 0.0;
			}

			if ((mode & LONGITUDE) && (LONGITUDE_POSITION[dbtype] != 0)) {
				record->longitude = IP2Location_readFloat(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (LONGITUDE_POSITION[dbtype]-1));
			} else {
				record->longitude = 0.0;
			}

			if ((mode & DOMAIN) && (DOMAIN_POSITION[dbtype] != 0)) {
				record->domain = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (DOMAIN_POSITION[dbtype]-1)));
			} else {
				record->domain = strdup(NOT_SUPPORTED);
			}

			if ((mode & ZIPCODE) && (ZIPCODE_POSITION[dbtype] != 0)) {
				record->zipcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (ZIPCODE_POSITION[dbtype]-1)));
			} else {
				record->zipcode = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & TIMEZONE) && (TIMEZONE_POSITION[dbtype] != 0)) {
				record->timezone = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (TIMEZONE_POSITION[dbtype]-1)));
			} else {
				record->timezone = strdup(NOT_SUPPORTED);
			}

			if ((mode & NETSPEED) && (NETSPEED_POSITION[dbtype] != 0)) {
				record->netspeed = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (NETSPEED_POSITION[dbtype]-1)));
			} else {
				record->netspeed = strdup(NOT_SUPPORTED);
			}
	
			if ((mode & IDDCODE) && (IDDCODE_POSITION[dbtype] != 0)) {
				record->iddcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (IDDCODE_POSITION[dbtype]-1)));
			} else {
				record->iddcode = strdup(NOT_SUPPORTED);
			}

			if ((mode & AREACODE) && (AREACODE_POSITION[dbtype] != 0)) {
				record->areacode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (AREACODE_POSITION[dbtype]-1)));
			} else {
				record->areacode = strdup(NOT_SUPPORTED);
			}

			if ((mode & WEATHERSTATIONCODE) && (WEATHERSTATIONCODE_POSITION[dbtype] != 0)) {
				record->weatherstationcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (WEATHERSTATIONCODE_POSITION[dbtype]-1)));
			} else {
				record->weatherstationcode = strdup(NOT_SUPPORTED);
			}

			if ((mode & WEATHERSTATIONNAME) && (WEATHERSTATIONNAME_POSITION[dbtype] != 0)) {
				record->weatherstationname = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (WEATHERSTATIONNAME_POSITION[dbtype]-1)));
			} else {
				record->weatherstationname = strdup(NOT_SUPPORTED);
			}

			if ((mode & MCC) && (MCC_POSITION[dbtype] != 0)) {
				record->mcc = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (MCC_POSITION[dbtype]-1)));
			} else {
				record->mcc = strdup(NOT_SUPPORTED);
			}

			if ((mode & MNC) && (MNC_POSITION[dbtype] != 0)) {
				record->mnc = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (MNC_POSITION[dbtype]-1)));
			} else {
				record->mnc = strdup(NOT_SUPPORTED);
			}

			if ((mode & MOBILEBRAND) && (MOBILEBRAND_POSITION[dbtype] != 0)) {
				record->mobilebrand = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (MOBILEBRAND_POSITION[dbtype]-1)));
			} else {
				record->mobilebrand = strdup(NOT_SUPPORTED);
			}

			if ((mode & ELEVATION) && (ELEVATION_POSITION[dbtype] != 0)) {
				record->elevation = IP2Location_readFloat(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (ELEVATION_POSITION[dbtype]-1));
			} else {
				record->elevation = 0.0;
			}
			
			if ((mode & USAGETYPE) && (USAGETYPE_POSITION[dbtype] != 0)) {
				record->usagetype = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + mid * (dbcolumn * 4 + 12) + 12 + 4 * (USAGETYPE_POSITION[dbtype]-1)));
			} else {
				record->usagetype = strdup(NOT_SUPPORTED);
			}
			return record;
		} else {
			if ( mp_int_compare(&ipno, &ipfrom) < 0 ) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	IP2Location_free_record(record);
	return NULL;
}

IP2LocationRecord *IP2Location_get_record(IP2Location *loc, char *ipstring, uint32_t mode)
{
	uint8_t dbtype = loc->databasetype;
	uint32_t ipno = IP2Location_ip2no(ipstring);
	FILE *handle = loc->filehandle;
	uint32_t baseaddr = loc->databaseaddr;
	uint32_t dbcount = loc->databasecount;
	uint32_t dbcolumn = loc->databasecolumn;

	uint32_t low = 0;
	uint32_t high = dbcount;
	uint32_t mid = 0;
	uint32_t ipfrom = 0;
	uint32_t ipto = 0;
	
	IP2LocationRecord *record = IP2Location_new_record();
	if (ipno == (uint32_t) MAX_IPV4_RANGE) {
		ipno = ipno - 1;
	}
	
	if(IP2Location_ip_is_ipv4(ipstring) == 0 ) {
		record->country_short = strdup(INVALID_IPV4_ADDRESS);
		record->country_long = strdup(INVALID_IPV4_ADDRESS);
		record->region = strdup(INVALID_IPV4_ADDRESS);
		record->city = strdup(INVALID_IPV4_ADDRESS);
		record->isp = strdup(INVALID_IPV4_ADDRESS);
		record->latitude = 0;
		record->longitude = 0;
		record->domain = strdup(INVALID_IPV4_ADDRESS);
		record->zipcode = strdup(INVALID_IPV4_ADDRESS);
		record->timezone = strdup(INVALID_IPV4_ADDRESS);
		record->netspeed = strdup(INVALID_IPV4_ADDRESS);
		record->iddcode = strdup(INVALID_IPV4_ADDRESS);
		record->areacode = strdup(INVALID_IPV4_ADDRESS);
		record->weatherstationcode = strdup(INVALID_IPV4_ADDRESS);
		record->weatherstationname = strdup(INVALID_IPV4_ADDRESS);
		record->mcc = strdup(INVALID_IPV4_ADDRESS);
		record->mnc = strdup(INVALID_IPV4_ADDRESS);
		record->mobilebrand = strdup(INVALID_IPV4_ADDRESS);
		record->elevation = 0;
		record->usagetype = strdup(INVALID_IPV4_ADDRESS);
		return record;
	}

	while (low <= high) 
	{
		mid = (uint32_t)((low + high)/2);
		ipfrom = IP2Location_read32(handle, baseaddr + mid * dbcolumn * 4);
		ipto 	= IP2Location_read32(handle, baseaddr + (mid + 1) * dbcolumn * 4);

		if ((ipno >= ipfrom) && (ipno < ipto)) 
		{
			if ((mode & COUNTRYSHORT) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_short = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1)));
			} else {
				record->country_short = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & COUNTRYLONG) && (COUNTRY_POSITION[dbtype] != 0)) {
				record->country_long = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (COUNTRY_POSITION[dbtype]-1))+3);
			} else {
				record->country_long = strdup(NOT_SUPPORTED);
			}

			if ((mode & REGION) && (REGION_POSITION[dbtype] != 0)) {
				record->region = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (REGION_POSITION[dbtype]-1)));
			} else {
				record->region = strdup(NOT_SUPPORTED);
			}

			if ((mode & CITY) && (CITY_POSITION[dbtype] != 0)) {
				record->city = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (CITY_POSITION[dbtype]-1)));
			} else {
				record->city = strdup(NOT_SUPPORTED);
			}

			if ((mode & ISP) && (ISP_POSITION[dbtype] != 0)) {
				record->isp = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ISP_POSITION[dbtype]-1)));
			} else {
				record->isp = strdup(NOT_SUPPORTED);
			}

			if ((mode & LATITUDE) && (LATITUDE_POSITION[dbtype] != 0)) {
				record->latitude = IP2Location_readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LATITUDE_POSITION[dbtype]-1));
			} else {
				record->latitude = 0.0;
			}

			if ((mode & LONGITUDE) && (LONGITUDE_POSITION[dbtype] != 0)) {
				record->longitude = IP2Location_readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (LONGITUDE_POSITION[dbtype]-1));
			} else {
				record->longitude = 0.0;
			}

			if ((mode & DOMAIN) && (DOMAIN_POSITION[dbtype] != 0)) {
				record->domain = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (DOMAIN_POSITION[dbtype]-1)));
			} else {
				record->domain = strdup(NOT_SUPPORTED);
			}

			if ((mode & ZIPCODE) && (ZIPCODE_POSITION[dbtype] != 0)) {
				record->zipcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ZIPCODE_POSITION[dbtype]-1)));
			} else {
				record->zipcode = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & TIMEZONE) && (TIMEZONE_POSITION[dbtype] != 0)) {
				record->timezone = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (TIMEZONE_POSITION[dbtype]-1)));
			} else {
				record->timezone = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & NETSPEED) && (NETSPEED_POSITION[dbtype] != 0)) {
				record->netspeed = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (NETSPEED_POSITION[dbtype]-1)));
			} else {
				record->netspeed = strdup(NOT_SUPPORTED);
			}
	
			if ((mode & IDDCODE) && (IDDCODE_POSITION[dbtype] != 0)) {
				record->iddcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (IDDCODE_POSITION[dbtype]-1)));
			} else {
				record->iddcode = strdup(NOT_SUPPORTED);
			}
	
			if ((mode & AREACODE) && (AREACODE_POSITION[dbtype] != 0)) {
				record->areacode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (AREACODE_POSITION[dbtype]-1)));
			} else {
				record->areacode = strdup(NOT_SUPPORTED);
			}
	
			if ((mode & WEATHERSTATIONCODE) && (WEATHERSTATIONCODE_POSITION[dbtype] != 0)) {
				record->weatherstationcode = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (WEATHERSTATIONCODE_POSITION[dbtype]-1)));
			} else {
				record->weatherstationcode = strdup(NOT_SUPPORTED);
			}
	
			if ((mode & WEATHERSTATIONNAME) && (WEATHERSTATIONNAME_POSITION[dbtype] != 0)) {
				record->weatherstationname = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (WEATHERSTATIONNAME_POSITION[dbtype]-1)));
			} else {
				record->weatherstationname = strdup(NOT_SUPPORTED);
			}

			if ((mode & MCC) && (MCC_POSITION[dbtype] != 0)) {
				record->mcc = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (MCC_POSITION[dbtype]-1)));
			} else {
				record->mcc = strdup(NOT_SUPPORTED);
			}

			if ((mode & MNC) && (MNC_POSITION[dbtype] != 0)) {
				record->mnc = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (MNC_POSITION[dbtype]-1)));
			} else {
				record->mnc = strdup(NOT_SUPPORTED);
			}

			if ((mode & MOBILEBRAND) && (MOBILEBRAND_POSITION[dbtype] != 0)) {
				record->mobilebrand = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (MOBILEBRAND_POSITION[dbtype]-1)));
			} else {
				record->mobilebrand = strdup(NOT_SUPPORTED);
			}
			
			if ((mode & ELEVATION) && (ELEVATION_POSITION[dbtype] != 0)) {
				record->elevation = IP2Location_readFloat(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (ELEVATION_POSITION[dbtype]-1));
			} else {
				record->elevation = 0.0;
			}
			
			if ((mode & USAGETYPE) && (USAGETYPE_POSITION[dbtype] != 0)) {
				record->usagetype = IP2Location_readStr(handle, IP2Location_read32(handle, baseaddr + (mid * dbcolumn * 4) + 4 * (USAGETYPE_POSITION[dbtype]-1)));
			} else {
				record->usagetype = strdup(NOT_SUPPORTED);
			}
			return record;
		} else {
			if ( ipno < ipfrom ) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}
	}
	IP2Location_free_record(record);
	return NULL;
}


IP2LocationRecord *IP2Location_new_record()
{
	IP2LocationRecord *record = (IP2LocationRecord *) malloc(sizeof(IP2LocationRecord));
	memset(record, 0, sizeof(IP2LocationRecord));
	return record;
}


void IP2Location_free_record(IP2LocationRecord *record)
{
	if (record->city != NULL)
		free(record->city);

	if (record->country_long != NULL)
		free(record->country_long);

	if (record->country_short != NULL)
		free(record->country_short);

	if (record->domain != NULL)
		free(record->domain);

	if (record->isp != NULL)
		free(record->isp);

	if (record->region != NULL)
		free(record->region);

	if (record->zipcode != NULL)
		free(record->zipcode);
		
	if (record->timezone != NULL)
		free(record->timezone);  
		
	if (record->netspeed != NULL)
		free(record->netspeed);  
	
	if (record->iddcode != NULL)
		free(record->iddcode);  

	if (record->areacode != NULL)
		free(record->areacode);  

	if (record->weatherstationcode != NULL)
		free(record->weatherstationcode);  

	if (record->weatherstationname != NULL)
		free(record->weatherstationname);  

	if (record->mcc != NULL)
		free(record->mcc);  

	if (record->mnc != NULL)
		free(record->mnc);  

	if (record->mobilebrand != NULL)
		free(record->mobilebrand);  
	
	// if (record->elevation != NULL)
		// free(record->elevation);  
	
	if (record->usagetype != NULL)
		free(record->usagetype);  

	free(record);
}

uint32_t IP2Location_ip2no(char* ipstring)
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

char* IP2Location_mp2string (mpz_t mp) {
	mpz_t test = mp;
	char *result = malloc(sizeof(char)*128);
	memset(result, 0, 128);
	mp_int_to_string(&mp, 10, result, 128);
	return result;
}

mpz_t IP2Location_ipv6_to_no(char* ipaddr) {
	
	char expanded[8];
	int isExpanded = 0;
	int padCount = 2;
	StringList* array = 0;
	StringList* iter = 0;
	unsigned int n = IP2Location_substr_count(":", ipaddr);
	mpz_t ipsub, mpResult;
	int subLoc = 8;

	mp_int_init(&ipsub);
	mp_int_init(&mpResult);
	memset(&expanded, 0, 8);
	if(n < 7) {
		expanded[0] = ':';
		expanded[1] = ':';
		while(n < 7) {
			expanded[padCount] = ':';
			padCount++;
			n++;
		}
		isExpanded = 1;
		ipaddr = IP2Location_replace("::", expanded, ipaddr);
	}

	array = IP2Location_split(":", ipaddr, DEFAULT, -1);
	iter = array;
	mp_int_init_value(&mpResult, 0);
	for(iter=array; iter!= 0; iter=iter->next ){
		subLoc--;
		if(strcmp(iter->data, "") == 0) 
			continue;

		mp_int_read_string(&ipsub, 16, iter->data);
		mp_int_mul_pow2(&ipsub, (16*subLoc), &ipsub);
		mp_int_add(&mpResult, &ipsub, &mpResult);
	}

	if(isExpanded)
		free(ipaddr);
	mp_int_clear(&ipsub);
	FreeStringList(array);
	return mpResult;
}

int IP2Location_ip_is_ipv4 (char* ipaddr) {
	unsigned int p;
	StringList* iparray = 0;
	StringList* ipsub = 0;
	for(p=0; p<strlen(ipaddr); p++) {
		if( ipaddr[p] >= '0' && ipaddr[p] <= '9' ||
			ipaddr[p] == '.' )
			continue;
		else
			return 0;
	}

	if(ipaddr[0] == '.' || ipaddr[strlen(ipaddr)-1] == '.') {
		return 0;
	}
	
	if( IP2Location_substr_count("::", ipaddr) > 0 ) {
		return 0;
	}

	iparray = IP2Location_split(".", ipaddr, DEFAULT, -1);
	if(StringListCount(iparray) != 4) {
		return 0;
	}
	
	for(ipsub = iparray; ipsub->next!=0; ipsub=ipsub->next) {
		if(atoi(ipsub->data) < 0 || atoi(ipsub->data) > 255) {
			return 0;	
		}
	}		
	FreeStringList(iparray);
	return 1;
}

int IP2Location_ip_is_ipv6 (char* ipaddr) {
	unsigned int n = 0;
	unsigned int k = 0;
	unsigned int m = 0;
	unsigned int p = 0;
	unsigned int checkFlag = 0;
	StringList* ipv6array = 0;
	StringList* ipsub = 0;

	n = IP2Location_substr_count(":", ipaddr); 
	if (n < 1 || n > 7) {
		return 0;
	}
	
	k = 0;
	ipv6array = IP2Location_split(":", ipaddr, DEFAULT, -1);
	for(ipsub = ipv6array; ipsub->next!=0; ipsub=ipsub->next)
	{
		k++;
		if (strcmp(ipsub->data, "") == 0) {
			continue;
		}
		
		if(strlen(ipsub->data) > 4) {
			return 0;
		}

		checkFlag = 1;
		for(p=0; p<strlen(ipsub->data); p++) {
			if( ipsub->data[p] >= '0' && ipsub->data[p] <= '9' ||
				ipsub->data[p] >= 'a' && ipsub->data[p] <= 'f' ||
				ipsub->data[p] >= 'A' && ipsub->data[p] <= 'F' )
				continue;
			else
				checkFlag = 0;
		}

		if(checkFlag)
			continue;

		if (k == n+1) {
			if (IP2Location_ip_is_ipv4(ipsub->data)) {
				// here we know it is embeded ipv4, should retrieve data from ipv4 db, pending...
				// the result of this will not be valid, since all characters are treated and calculated 
				// in hex based.
				// In addition, embeded ipv4 requires 96 '0' bits. We need to check this too.
				continue;	
			}
		}
		return 0;
	}
	
	m = IP2Location_substr_count("::", ipaddr);
	if (m > 1 && n < 7) {
		return 0;
	}
	return 1;
}

StringList* IP2Location_split(char* delimiters, char* targetString, unsigned int flags, int limit)
{
	StringList* tokenHead = 0;
	StringList* prevToken = 0;
	StringList* tokenStore = 0;
	char* targetCopy = malloc(sizeof(char)*(strlen(targetString)+1));
	char* token = 0;	
	unsigned int match, i, j = 0;
	strcpy(targetCopy, targetString);	
	
	token = targetCopy;
	match = 0;
	for(i=0; i<strlen(targetCopy); i++) {
		if(match) {
			match = 0;
			token = targetCopy+i;
		}
		
		for(j=0; j<strlen(delimiters); j++) {
			if(targetCopy[i] == delimiters[j]) {
				match = 1;
				targetCopy[i] = 0;
				break;
			}
		}
		
		if(!match && ((i+1)!=strlen(targetCopy)))
			continue;

		add:
		tokenStore = malloc(sizeof(StringList));
		tokenStore->data = malloc(sizeof(char)*(strlen(token)+1));
		tokenStore->next = 0;
		if(prevToken == 0)
			tokenHead = tokenStore;
		else
			prevToken->next = tokenStore;

		prevToken = tokenStore;
		strcpy(tokenStore->data, token);
		targetCopy[i] = delimiters[j];

		if(match && ((i+1)==strlen(targetCopy)))
		{
			token = targetString+strlen(targetString);
			match = 0;
			goto add;
		}
	}
	
	free(targetCopy);
	return tokenHead;
}

unsigned int IP2Location_substr_count(char* substr, char* targetString) 
{
	unsigned int count = 0;
	char* caret = targetString;
	while( (caret = strstr(caret, substr)) != 0 ) {
		count++;
		caret = caret + strlen(substr);
	}
    return count;	
}

char* IP2Location_replace(char* substr, char* replace, char* targetString)
{
	//estimate max possible replaced string length
	char* buff = malloc(sizeof(char) * ( (int)(strlen(targetString)/strlen(substr) + 0.5) * strlen(replace)));
	char* headCaret = targetString;
	char* endCaret = 0;
	buff[0] = 0;
	
	while( (endCaret = strstr(headCaret, substr)) != 0) {
		strncat(buff, headCaret, endCaret-headCaret);
		strcat(buff, replace);
		headCaret = endCaret + strlen(substr);
	}
	strcat(buff, headCaret);
	return buff;
}

void FreeStringList (StringList* toFree) {
	if(toFree != 0) {
		if(toFree->data != 0)
			free(toFree->data);
		FreeStringList(toFree->next);
		free(toFree);
	}
}

unsigned int StringListCount (StringList* toCount) {
	unsigned int count=0;
	while(toCount!=0) {
		count++;
		toCount = toCount->next;
	}
	return count;
}
