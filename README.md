# IP2Location C Library

IP2Location is a C library that enables the user to find the country, region, city, latitude, longitude, 
zip code, time zone, ISP, domain name, connection type, area code, weather, mobile network, elevation, 
usage type by IP address or hostname originates from. The library reads the geo location information
from **IP2Location BIN data** file.

Supported IPv4 and IPv6 address.

For more details, please visit:
[https://www.ip2location.com/developers/c](https://www.ip2location.com/developers/c)

# Installation
###  Unix/Linux
    autoreconf -i -v --force
    ./configure
    make
    make install
    cd data
    perl ip-country.pl

### Windows
    Execute "vcvarsall.bat". (This file is part of Microsoft Visual C, not ip2location code) 
    nmake -f Makefile.win
    cd data
    perl ip-country.pl

### MacOS
    autoreconf -i -v --force
    export CFLAGS=-I/usr/include/malloc 
    ./configure
    make
    make install
    cd data
    perl ip-country.pl

# Testing
    cd test
    test-IP2Location

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

# Packages for Fedora/Enterprise Linux can be found here
https://koji.fedoraproject.org/koji/packageinfo?packageID=32132
https://src.fedoraproject.org/rpms/IP2Location

# Support
Email: support@ip2location.com.  
URL: [https://www.ip2location.com](https://www.ip2location.com)
