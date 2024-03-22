# Quickstart

## Dependencies

This library requires IP2Location BIN database to function. You may
download the BIN database at

-   IP2Location LITE BIN Data (Free): <https://lite.ip2location.com>
-   IP2Location Commercial BIN Data (Comprehensive):
    <https://www.ip2location.com>

## Installation

### Unix/Linux
```
    autoreconf -i -v --force
    ./configure
    make
    make install
    cd data
    perl ip-country.pl
```



### Debian

```
curl -LO https://github.com/chrislim2888/IP2Location-C-Library/releases/download/8.6.1/ip2location-8.6.1.deb
sudo dpkg -i ip2location-8.6.1.deb
```



### Ubuntu

```
sudo add-apt-repository ppa:ip2location/ip2location
sudo apt update
sudo apt install ip2location
```



### Windows

```
    Execute "vcvarsall.bat". (This file is part of Microsoft Visual C, not ip2location code) 
    nmake -f Makefile.win
    cd data
    perl ip-country.pl
```

### MacOS

```
    autoreconf -i -v --force
    export CFLAGS=-I/usr/include/malloc 
    ./configure
    make
    make install
    cd data
    perl ip-country.pl
```

# Testing

```
    cd test
    test-IP2Location
```

## Sample Codes

### Query geolocation information from BIN database

You can query the geolocation information from the IP2Location BIN
database as below:

``` php
#include <IP2Location.h>
#include <string.h>
#include <stdio.h>

int main () {
	char ipAddress[30];

#ifdef WIN32
	IP2Location *IP2LocationObj = IP2Location_open("..\\data\\IP-COUNTRY.BIN");
#else
	IP2Location *IP2LocationObj = IP2Location_open("../data/IP-COUNTRY.BIN");
#endif
	IP2LocationRecord *record = NULL;
	
	printf("IP2Location API version: %s \n", IP2Location_api_version_string());

	if (IP2LocationObj == NULL)
	{
		printf("Please install the database in correct path.\n");
		return -1;
	}

	fprintf(stdout,"IP2Location BIN version: %s\n", IP2Location_bin_version(IP2LocationObj));
	
	IP2Location_open_mem(IP2LocationObj, IP2LOCATION_SHARED_MEMORY);
	
	IP2LocationRecord *record = IP2Location_get_all(IP2LocationObj, "161.11.12.13");
	
	printf("%s %s %s %s %s %f %f %s %s %s %s %s %s %s %s %s %s %s %f %s\n", record->country_short, record->country_long, record->region, record->city, record->isp, record->latitude, record->longitude, record->domain, record->zipcode, record->timezone, record->netspeed, record->iddcode, record->areacode, record->weatherstationcode, record->weatherstationname, record->mcc, record->mnc, record->mobilebrand, record->elevation, record->usagetype, record->address_type, record->category, record->district, record->asn, record->as);
	
	IP2Location_free_record(record);
	
	IP2Location_close(IP2LocationObj);
	
	/*Below call will delete the shared memory unless if any other process is attached it. 
	 *if any other process is attached to it, shared memory will be closed when last process
	 *attached to it closes the shared memory 
	 *If any process call e, next process which IP2Location_open_mem
	 *with shared memory option, will open the new shared memory.Deleted memory will not be available for
	 * any new process but will be accesible for the processes which are already using it. 
	 */
	IP2Location_delete_shm();
}

```