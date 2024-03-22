# IP2Location C Library

IP2Location is a C library that enables the user to find the country, region or state, district, city, latitude and longitude,  ZIP/Postal code, time zone, Internet Service Provider (ISP) or company  name, domain name, net speed, area code, weather station code, weather  station name, mobile country code (MCC), mobile network code (MNC) and  carrier brand, elevation, usage type, address type, IAB category and ASN by IP address or hostname originates from. The library reads the geo location information
from **IP2Location BIN data** file.

In addition to the C Library, the build process will also create an executable called "**ip2location**" that allows you to directly perform IP geolocation lookups. Please refer to the ip2location CLI documentation for more information.
Supported IPv4 and IPv6 address.

For more details, please visit:
[https://www.ip2location.com/developers/c](https://www.ip2location.com/developers/c)

# Developer Documentation
To learn more about installation, usage, and code examples, please visit the developer documentation at [https://ip2location-c-library.readthedocs.io/en/latest/index.html.](https://ip2location-c-library.readthedocs.io/en/latest/index.html)
# Installation

Please refer to the [installation section](https://ip2location-c-library.readthedocs.io/en/latest/quickstart.html#installation) in our documentation for instruction on how to install in various platform.

# Sample BIN Databases
* Download free IP2Location LITE databases at [https://lite.ip2location.com](https://lite.ip2location.com)  
* Download IP2Location sample databases at [https://www.ip2location.com/developers](https://www.ip2location.com/developers)

# IPv4 BIN vs IPv6 BIN
* Use the IPv4 BIN file if you just need to query IPv4 addresses.
* Use the IPv6 BIN file if you need to query BOTH IPv4 and IPv6 addresses.

# NOTES
If you encountered **error while loading shared libraries** in linux platform, that could be due to the shared library was saved into **usr/local/lib** that might not be the default library path. You can run the below command
```
ldconfig /usr/local/lib
```

# IP2Location CLI
Query an IP address and display the result

```
ip2location -d [IP2LOCATION BIN DATA PATH] --ip [IP ADDRESS]
```

Query all IP addresses from an input file and display the result
```
ip2location -d [IP2LOCATION BIN DATA PATH] -i [INPUT FILE PATH]
```

Query all IP addresses from an input file and display the result in XML format
```
ip2location -d [IP2LOCATION BIN DATA PATH] -i [INPUT FILE PATH] --format XML
```


# References

##### Address Type

Address type is the IP address types as defined in Internet Protocol version 4 (IPv4) and Internet Protocol version 6 (IPv6). The database return a single character. For the full description, please refer to list below:

- (A) Anycast - One to the closest
- (U) Unicast - One to one
- (M) Multicast - One to multiple
- (B) Broadcast - One to all



##### Category

Category is the domain category is based on [IAB Tech Lab Content Taxonomy](https://www.ip2location.com/free/iab-categories). These categories are comprised of Tier-1 and Tier-2 (if available) level categories widely used in services like advertising, Internet security  and filtering appliances. Please refer to https://www.ip2location.com/free/iab-categories for the full list.



# Packages for Fedora/Enterprise Linux can be found here

* https://koji.fedoraproject.org/koji/packageinfo?packageID=32132
* https://src.fedoraproject.org/rpms/IP2Location
* https://bodhi.fedoraproject.org/updates/?packages=IP2Location

# Support
Email: support@ip2location.com.  
URL: [https://www.ip2location.com](https://www.ip2location.com)
