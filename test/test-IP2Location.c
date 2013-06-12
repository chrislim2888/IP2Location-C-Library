/* Copyright (C) 2005-2013 IP2Location.com
 * All Rights Reserved
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>.
 */

#include <IP2Location.h>
#include <string.h>

int main () {
  FILE *f;
  char ipAddress[30];
  char expectedCountry[3];
  int failed = 0;
  int test_num = 1;

#ifdef WIN32
  IP2Location *IP2LocationObj = IP2Location_open("..\\data\\IP-COUNTRY-SAMPLE.BIN");
#else
  IP2Location *IP2LocationObj = IP2Location_open("../data/IP-COUNTRY-SAMPLE.BIN");
#endif
	IP2LocationRecord *record = NULL;
	
	if (IP2LocationObj == NULL)
	{
		printf("Please install the database in correct path.\n");
		return -1;
	}

	if(IP2Location_open_mem(IP2LocationObj, IP2LOCATION_SHARED_MEMORY) == -1)
	{
		fprintf(stderr, "IPv4: Call to IP2Location_open_mem failed\n");
	}
	
#ifdef WIN32
	f = fopen("country_test_ipv4_data.txt","r");
#else
	f = fopen("country_test_ipv4_data.txt","r");
#endif

	while (fscanf(f, "%s", ipAddress) != EOF) {
		fscanf(f, "%s", expectedCountry);
		record = IP2Location_get_all(IP2LocationObj, ipAddress);
		if (record != NULL)	{
			if (strcmp(expectedCountry,record->country_short) != 0) {
				fprintf(stderr,"Test IP Address %s (Test %d) failed. We got %s but expected %s,\n",ipAddress,test_num,record->country_short,expectedCountry);
				failed++;
			}
			IP2Location_free_record(record);
			test_num++;
		} else {
			
		}
	}
	fclose(f);
	
	IP2Location_close(IP2LocationObj);
	/*Below call will delete the shared memory unless if any other process is attached it. 
	 *if any other process is attached to it, shared memory will be closed when last process
	 *attached to it closes the shared memory 
	 *If any process call IP2Location_delete_shm, next process which IP2Location_open_mem
	 *with shared memory option, will open the new shared memory.Deleted memory will not be available for
	 * any new process but will be accesible for the processes which are already using it. 
	 */
	IP2Location_delete_shm();
	if ((test_num > 1) && (failed == 0)) {
		fprintf(stdout, "IP2Location IPv4 Testing passed.\n");
	}
	
	/* --- IPv6 Testing --- */
#ifdef WIN32
	IP2LocationObj = IP2Location_open("..\\data\\IPV6-COUNTRY.BIN");
#else
	IP2LocationObj = IP2Location_open("../data/IPV6-COUNTRY.BIN");
#endif
	if ( IP2Location_open_mem(IP2LocationObj, IP2LOCATION_FILE_IO) == -1 )
	{
		fprintf(stderr, "IPv6: Call to IP2Location_open_mem failed\n");
	}	
	record = NULL;
	if (IP2LocationObj == NULL)
	{
		printf("Please install the database in correct path.\n");
		return -1;
	}

	f = fopen("country_test_ipv6_data.txt","r");
	
	while (fscanf(f, "%s", ipAddress) != EOF) {
		fscanf(f, "%s", expectedCountry);
		record = IP2Location_get_all(IP2LocationObj, ipAddress);
		if (strcmp(expectedCountry,record->country_short) != 0) {
			fprintf(stderr,"Test IP Address %s (Test %d) failed. We got %s but expected %s,\n",ipAddress,test_num,record->country_short,expectedCountry);
			failed++;
		}
		IP2Location_free_record(record);
		test_num++;
	}
	
	fclose(f);
	
	IP2Location_close(IP2LocationObj);
	
	if ((test_num > 1) && (failed == 0)) {
		fprintf(stdout, "IP2Location IPv6 Testing passed.\n");
	}
		
	return failed;
}
