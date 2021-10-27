#include <string.h>
#include <stdbool.h>
#include <IP2Location.h>

static void print_usage(const char *argv0)
{
	printf(
"ip2location -p [IP ADDRESS] -d [IP2LOCATION BIN DATA PATH] [OPTIONS]\n"
"	-b, --bin-version\n"
"		Print the IP2Location BIN database version.\n"
"\n"
"	-d, --data-file\n"
"		Specify the path of IP2Location BIN data file.\n"
"\n"
"   -e, --field\n"
"		Output the field data.\n"
"		Field name includes:\n"
"			country_code	 \n"
"			Two-character country code based on ISO 3166.\n"
"\n"
"			country_name\n"
"			Country name based on ISO 3166.\n"
"\n"
"			region_name\n"
"			Region or state name.\n"
"\n"
"			city_name\n"
"			City name.\n"
"\n"
"			latitude\n"
"			City latitude. Defaults to capital city latitude if the city is unknown.\n"
"\n"
"			longitude\n"
"			City longitude. Defaults to capital city longitude if the city is unknown.\n"
"\n"
"			zip_code\n"
"			ZIP/Postal code (170 countries supported).\n"
"\n"
"			time_zone\n"
"			UTC time zone (with DST supported).\n"
"\n"
"			isp\n"
"			Internet Service Provider or company's name.\n"
"\n"
"			domain\n"
"			Internet domain name associated with IP address range.\n"
"\n"
"			net_speed\n"
"			Internet connection type.\n"
"\n"
"			idd_code\n"
"			The IDD prefix to call the city from another country.\n"
"\n"
"			area_code\n"
"			A varying length number assigned to geographic areas for calls between cities (221 countries supported).\n"
"\n"
"			weather_station_code\n"
"			The special code to identify the nearest weather observation station.\n"
"\n"
"			weather_station_name\n"
"			The name of the nearest weather observation station.\n"
"\n"
"			mcc\n"
"			Mobile Country Codes (MCC) as defined in ITU E.212 for use in identifying mobile stations in wireless telephone networks, particularly GSM and UMTS networks.\n"
"\n"
"			mnc\n"
"			Mobile Network Code (MNC) is used in combination with a Mobile Country Code (MCC) to uniquely identify a mobile phone operator or carrier.\n"
"\n"
"			mobile_brand\n"
"			Commercial brand associated with the mobile carrier. You may click here to view the coverage report.\n"
"\n"
"			elevation\n"
"			Average height of the city above sea level in meters (m).\n"
"\n"
"			usage_type\n"
"			Usage type classification of ISP or company.\n"
"\n"
"			address_type\n"
"			IP address types as defined in Internet Protocol version 4 (IPv4) and Internet Protocol version 6 (IPv6).\n"
"\n"
"			category\n"
"			The domain category is based on IAB Tech Lab Content Taxonomy.\n"
"\n"
"	-f, --format\n"
"	Output format. Supported format:\n"
"		- csv (default)\n"
"		- tab\n"
"		- xml\n"
"\n"
"	-h, -?, --help\n"
"	Display the help.\n"
"\n"
"	-i, --input-file\n"
"	Specify an input file of IP address list, one IP per row.\n"
"\n"
"	-n, --no-heading\n"
"	Suppress the heading display.\n"
"\n"
"	-o, --output-file\n"
"	Specify an output file to store the lookup results.\n"
"\n"
"	-p, --ip\n"
"	Specify an IP address query (Supported IPv4 and IPv6 address).\n"
"\n"
"	-v, --version\n"
"	Print the version of the IP2Location version.\n");
}

static void print_version()
{
	printf("IP2Location version 8.0.0\n");
}

static void print_footer(FILE *fout, const char *field, const char *format)
{
	if (strcmp(format, "XML") == 0) {
		fprintf(fout, "</xml>\n");
		return;
	}
}

static void print_header(FILE *fout, const char *field, const char *format)
{
	const char *start = field;
	const char *end = strchr(start, ',');
	int first = 1;

	if (strcmp(format, "XML") == 0) {
		fprintf(fout, "<xml>\n");
		return;
	}

#define WRITE_HEADER(field_name)  \
		if (strncmp(start, field_name, end - start) == 0) { \
			if (strcmp(format, "CSV") == 0) { \
				if (!first) { \
					fprintf(fout, ","); \
				} \
				fprintf(fout, "\"%s\"", field_name); \
			} else if (strcmp(format, "TAB") == 0) { \
				if (!first) { \
					fprintf(fout, "\t"); \
				} \
				fprintf(fout, "%s", field_name); \
			} \
			first = 0; \
		}
	for (;;) {
		if (end == NULL) {
			end = start + strlen(start);
		}

		WRITE_HEADER("ip");
		WRITE_HEADER("country_code");
		WRITE_HEADER("country_name");
		WRITE_HEADER("region_name");
		WRITE_HEADER("city_name");
		WRITE_HEADER("isp");
		WRITE_HEADER("latitude");
		WRITE_HEADER("longitude");
		WRITE_HEADER("domain");
		WRITE_HEADER("zip_code");
		WRITE_HEADER("time_zone");
		WRITE_HEADER("net_speed");
		WRITE_HEADER("idd_code");
		WRITE_HEADER("area_code");
		WRITE_HEADER("weather_station_code");
		WRITE_HEADER("weather_station_name");
		WRITE_HEADER("mcc");
		WRITE_HEADER("mnc");
		WRITE_HEADER("mobile_brand");
		WRITE_HEADER("elevation");
		WRITE_HEADER("usage_type");
		WRITE_HEADER("address_type");
		WRITE_HEADER("category");

		if (*end == ',') {
			start = end + 1;
			end = strchr(start, ',');
		} else {
			break;
		}
	}
	fprintf(fout, "\n");
}


static void print_record(FILE *fout, const char *field, IP2LocationRecord *record, const char *format, const char *ip)
{
	const char *start = field;
	const char *end = strchr(start, ',');
	int first = 1;

	if (strcmp(format, "XML") == 0) {
		fprintf(fout, "<row>");
	}

#define WRITE_FIELD(field_name, field)  \
		if (strncmp(start, field_name, end - start) == 0) { \
			const char *value = field; \
			if (strcmp(value, NOT_SUPPORTED) == 0) { \
				value = "N/A"; \
			} \
			if (strcmp(format, "XML") == 0) { \
				fprintf(fout, "<%s>%s</%s>", field_name, value, field_name); \
			} else if (strcmp(format, "CSV") == 0) { \
				if (!first) { \
					fprintf(fout, ","); \
				} \
				fprintf(fout, "\"%s\"", value); \
			} else if (strcmp(format, "TAB") == 0) { \
				if (!first) { \
					fprintf(fout, "\t"); \
				} \
				fprintf(fout, "%s", value); \
			} \
			first = 0; \
		}
#define WRITE_FIELDF(field_name, field)  \
		if (strncmp(start, field_name, end - start) == 0) { \
			if (strcmp(format, "XML") == 0) { \
				fprintf(fout, "<%s>%f</%s>", field_name, field, field_name); \
			} else if (strcmp(format, "CSV") == 0) { \
				if (!first) { \
					fprintf(fout, ","); \
				} \
				fprintf(fout, "\"%f\"", field); \
			} else if (strcmp(format, "TAB") == 0) { \
				if (!first) { \
					fprintf(fout, "\t"); \
				} \
				fprintf(fout, "%f", field); \
			} \
			first = 0; \
		}


	for (;;) {
		if (end == NULL) {
			end = start + strlen(start);
		}

		WRITE_FIELD("ip", ip);
		WRITE_FIELD("country_code", record->country_short);
		WRITE_FIELD("country_name", record->country_long);
		WRITE_FIELD("region_name", record->region);
		WRITE_FIELD("city_name", record->city);
		WRITE_FIELD("isp", record->isp);
		WRITE_FIELDF("latitude", record->latitude);
		WRITE_FIELDF("longitude", record->longitude);
		WRITE_FIELD("domain", record->domain);
		WRITE_FIELD("zip_code", record->zipcode);
		WRITE_FIELD("time_zone", record->timezone);
		WRITE_FIELD("net_speed", record->netspeed);
		WRITE_FIELD("idd_code", record->iddcode);
		WRITE_FIELD("area_code", record->areacode);
		WRITE_FIELD("weather_station_code", record->weatherstationcode);
		WRITE_FIELD("weather_station_name", record->weatherstationname);
		WRITE_FIELD("mcc", record->mcc);
		WRITE_FIELD("mnc", record->mnc);
		WRITE_FIELD("mobile_brand", record->mobilebrand);
		WRITE_FIELDF("elevation", record->elevation);
		WRITE_FIELD("usage_type", record->usagetype);
		WRITE_FIELD("address_type", record->address_type);
		WRITE_FIELD("category", record->category);

		if (*end == ',') {
			start = end + 1;
			end = strchr(start, ',');
		} else {
			break;
		}
	}
	if (strcmp(format, "XML") == 0) {
		fprintf(fout, "</row>");
	}
	fprintf(fout, "\n");
}

int main(int argc, char *argv[])
{
	int i;
	char *data_file = NULL;

	const char *input_file = NULL;
	const char *output_file = NULL;
	const char *ip = NULL;
	const char *format = "CSV";
	const char *field = NULL;
	int no_heading = 0;
	bool print_bin_version = false;
	IP2Location *obj = NULL;
	IP2LocationRecord *record = NULL;
	FILE *fout = stdout;

	field = "ip,country_code,country_name,region_name,city_name,isp,latitude,longitude,domain,zip_code,time_zone,net_speed,idd_code,area_code,weather_station_code,weather_station_name,mcc,mnc,mobile_brand,elevation,usage_type,address_type,category";

	for (i = 1; i < argc; i++) {
		const char *argvi = argv[i];

		if (strcmp(argvi, "-d") == 0 || strcmp(argvi, "--data-file") == 0) {
			if (i + 1 < argc) {
				data_file = argv[++i];
			}
		} else if (strcmp(argvi, "-i") == 0 || strcmp(argvi, "--input-file") == 0) {
			if (i + 1 < argc) {
				input_file = argv[++i];
			}
		} else if (strcmp(argvi, "-b") == 0 || strcmp(argvi, "--bin-version") == 0) {
			print_bin_version = true;
		} else if (strcmp(argvi, "-p") == 0 || strcmp(argvi, "--ip") == 0) {
			if (i + 1 < argc) {
				ip = argv[++i];
			}
		} else if (strcmp(argvi, "-o") == 0 || strcmp(argvi, "--output-file") == 0) {
			if (i + 1 < argc) {
				output_file = argv[++i];
			}
		} else if (strcmp(argvi, "-f") == 0 || strcmp(argvi, "--format") == 0) {
			if (i + 1 < argc) {
				format = argv[++i];
			}
		} else if (strcmp(argvi, "-h") == 0 || strcmp(argvi, "-?") == 0 || strcmp(argvi, "--help") == 0) {
			print_usage(argv[0]);
			return 0;
		} else if (strcmp(argvi, "-v") == 0 || strcmp(argvi, "--version") == 0) {
			print_version();
			return 0;
		} else if (strcmp(argvi, "-e") == 0 || strcmp(argvi, "--field") == 0) {
			if (i + 1 < argc) {
				field = argv[++i];
			}
		} else if (strcmp(argvi, "-n") == 0 || strcmp(argvi, "--no-heading") == 0) {
			no_heading = 1;
		}
	}

	if (strcmp(format, "CSV") != 0 && strcmp(format, "XML") != 0 && strcmp(format, "TAB") != 0) {
		fprintf(stderr, "Invalid format %s, supported formats: CSV, XML, TAB\n", format);
		exit(-1);
	}

	if (data_file == NULL) {
		fprintf(stderr, "Datafile is absent\n");
		exit(-1);
	}
	
	obj = IP2Location_open((char *)data_file);
	if (obj == NULL) {
		fprintf(stderr, "Failed to open BIN database %s\n", data_file);
		exit(-1);
	}

	if (print_bin_version) {
		printf("BIN version %s\n", IP2Location_bin_version(obj));
		exit(0);
	}

	if (output_file != NULL) {
		fout = fopen(output_file, "w");
		if (fout == NULL) {
			fprintf(stderr, "Failed to open output file %s\n", output_file);
			exit(-1);
		}
	}

	if (!no_heading) {
		print_header(fout, field, format);
	}

	if (ip != NULL) {
		record = IP2Location_get_all(obj, (char *)ip);
		print_record(fout, field, record, format, ip);
		IP2Location_free_record(record);
	}

	if (input_file != NULL) {
		char *line = NULL;
		size_t n;
		ssize_t len;
		FILE *fin = fopen(input_file, "r");

		if (fin == NULL) {
			fprintf(stderr, "Failed to open input file %s\n", input_file);
			exit(-1);
		}

		while ((len = getline(&line, &n, fin)) != -1) {
			if (line[len - 1] == '\n') {
				line[--len] = '\0';
			}
			if (line[len - 1] == '\r') {
				line[--len] = '\0';
			}
			record = IP2Location_get_all(obj, line);
			print_record(fout, field, record, format, line);
			IP2Location_free_record(record);
		}

		fclose(fin);
	}

	if (!no_heading) {
		print_footer(fout, field, format);
	}
	
	IP2Location_close(obj);

	return 0;
}
