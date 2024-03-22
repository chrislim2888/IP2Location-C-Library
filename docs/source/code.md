# IP2Location C API

## Database Class

```{py:function} IP2Location_open(filename)
Load the IP2Location BIN database.

:param char file: (Required) The file path links to IP2Location BIN databases.
:return: Returns a struct contains the metadata of the IP2Location BIN database.
:rtype: struct
```

```{py:function} IP2Location_open_mem(handler, mode)
Set the DB access type.

:param struct handler: (Required) The metadata of the IP2Location BIN databases.
:param enum mode: (Required) The lookup mode of IP2Location BIN databases. Available options are IP2LOCATION_FILE_IO, IP2LOCATION_CACHE_MEMORY, IP2LOCATION_SHARED_MEMORY.
```

```{py:function} IP2Location_close(handler)
Close the IP2Location database file.

:param struct handler: (Required) The metadata of the IP2Location BIN databases.
```

```{py:function} IP2Location_api_version_string()
Return the API version of IP2Location C library.

:return: Returns the API version of IP2Location C library.
:rtype: array char
```

```{py:function} IP2Location_bin_version()
Return the IP2Location BIN database version.

:return: Returns the IP2Location BIN database version.
:rtype: array char
```

```{py:function} IP2Location_free_record(record)
Close the IP2Location database file.

:param struct record: (Required) The result returned by IP2Location_get_all class.
```

```{py:function} IP2Location_get_all(ip)
Retrieve geolocation information for an IP address.

:param str ip: (Required) The IP address (IPv4 or IPv6).
:return: Returns the geolocation information in dict. Refer below table for the fields avaliable in the dict
:rtype: array

**RETURN FIELDS**

| Field Name       | Description                                                  |
| ---------------- | ------------------------------------------------------------ |
| country_short    |     Two-character country code based on ISO 3166. |
| country_long     |     Country name based on ISO 3166. |
| region           |     Region or state name. |
| city             |     City name. |
| isp              |     Internet Service Provider or company\'s name. |
| latitude         |     City latitude. Defaults to capital city latitude if city is unknown. |
| longitude        |     City longitude. Defaults to capital city longitude if city is unknown. |
| domain           |     Internet domain name associated with IP address range. |
| zipcode          |     ZIP code or Postal code. [172 countries supported](https://www.ip2location.com/zip-code-coverage). |
| timezone         |     UTC time zone (with DST supported). |
| netspeed         |     Internet connection type. |
| iddcode         |     The IDD prefix to call the city from another country. |
| areacode        |     A varying length number assigned to geographic areas for calls between cities. [223 countries supported](https://www.ip2location.com/area-code-coverage). |
| weatherstationcode     |     The special code to identify the nearest weather observation station. |
| weatherstationname     |     The name of the nearest weather observation station. |
| mcc              |     Mobile Country Codes (MCC) as defined in ITU E.212 for use in identifying mobile stations in wireless telephone networks, particularly GSM and UMTS networks. |
| mnc              |     Mobile Network Code (MNC) is used in combination with a Mobile Country Code(MCC) to uniquely identify a mobile phone operator or carrier. |
| mobilebrand     |     Commercial brand associated with the mobile carrier. You may click [mobile carrier coverage](https://www.ip2location.com/mobile-carrier-coverage) to view the coverage report. |
| elevation        |     Average height of city above sea level in meters (m). |
| usagetype       |     Usage type classification of ISP or company. |
| address_type     |     IP address types as defined in Internet Protocol version 4 (IPv4) and Internet Protocol version 6 (IPv6). |
| category         |     The domain category based on [IAB Tech Lab Content Taxonomy](https://www.ip2location.com/free/iab-categories). |
| district         |     District or county name. |
| asn              |     Autonomous system number (ASN). BIN databases. |
| as               |     Autonomous system (AS) name. |
```