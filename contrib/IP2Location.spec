%define	version 8.0.4

Name:		IP2Location
Summary:	C library for mapping IP address to geolocation information
Version:	%{version}
Release:	5%{?dist}
Group:		System Environment/Libraries
License:	LGPLv3
URL:		http://www.ip2location.com/
Source0:	https://github.com/chrislim2888/IP2Location-C-Library/archive/%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	libtool


%description
IP2Location C library enables the user to get the country, region, city,
coordinates, ZIP code, time zone, ISP, domain name, connection type,
area code, weather info, mobile carrier, elevation and usage type from any IP
address or hostname. This library has been optimized for speed and memory
utilization. The library contains API to query all IP2Location LITE and
commercial binary databases.

Users can download the latest LITE database from IP2Location web site using e.g.
the included downloader.


%package 	devel
Summary:	Static library and header files for the ip2location library
Group:		Development/C
Requires:	%{name} = %{version}-%{release}
Provides:	%{name}-devel = %{version}-%{release}

%description 	devel
IP2Location C library enables the user to get the country, region, city,
coordinates, ZIP code, time zone, ISP, domain name, connection type,
area code, weather info, mobile carrier, elevation and usage type from any IP
address or hostname. This library has been optimized for speed and memory
utilization. The library contains API to query all IP2Location LITE and
commercial binary databases.

This package contains the development files for the IP2Location library.


%prep
%setup -q -n IP2Location-C-Library-%{version}

# remove a warning option which break configure on older gcc versions
# (at least gcc version 4.1.2 20080704)
perl -pi -e 's/-Wno-unused-result//' configure.ac


%build
sh ./bootstrap
autoreconf -fi

%configure --disable-static
make clean
make %{?_smp_mflags} COPTS="$RPM_OPT_FLAGS"

# convert CSV to BIN
make -C data convert

%check
LD_LIBRARY_PATH=%{buildroot}%{_libdir}:$LD_LIBRARY_PATH make check


%install
rm -rf %{buildroot}

make install DESTDIR=%{buildroot}

# cleanup
rm -f %{buildroot}%{_libdir}/*.*a

# tools
install -d %{buildroot}%{_datadir}/%{name}/tools
cp tools/download.pl %{buildroot}%{_datadir}/%{name}/tools

# database directory
install -d %{buildroot}%{_datadir}/%{name}/


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%clean
rm -rf %{buildroot}

%files
%defattr(644,root,root,755)

%doc AUTHORS ChangeLog COPYING README LICENSE.GPLv3 NEWS INSTALL LICENSE.LGPLv3

%attr(755,-,-) %{_libdir}/*.so*

%attr(755,-,-) %{_datadir}/%{name}/tools/*

%dir %{_datadir}/%{name}/


%files devel
%defattr(-,root,root)
%{_includedir}/*

%doc Developers_Guide.txt


%changelog
* Sun Feb 26 2017 Peter Bieringer <pb@bieringer.de> - 8.0.4-5
- update to 8.0.4
- add some fixes related to move to github
- integrate download.pl into github tree

* Sun May 03 2015 Peter Bieringer <pb@bieringer.de> - 7.0.1-4
- add Developers_Guide.txt to doc/devel
- change group of base package to System Environment/Libraries
- add check/post/postuninstall section
- migrate some settings from http://www.ip2location.com/rpm/ip2location-c.spec

* Fri Apr 17 2015 Peter Bieringer <pb@bieringer.de> - 7.0.1-3
- update to 7.0.1
- add ip2location-downloader/download.pl

* Thu Apr 16 2015 Peter Bieringer <pb@bieringer.de>
- update to 7.0.0

* Sat Jan 24 2015 Peter Bieringer <pb@bieringer.de>
- run "make clean" before "make" cleanup i368 objects containend in source code

* Sun Jul 20 2014 Peter Bieringer <pb@bieringer.de>
- adjustments for 6.0.2

* Thu Aug 22 2013 Peter Bieringer <pb@bieringer.de>
- adjustments for 6.0.1, update license version
- some RPM fixes

* Sun May 15 2011 Oden Eriksson <oeriksson@mandriva.com> 4.0.2-1mdv2011.0
+ Revision: 674881
- import ip2location

* Sun May 15 2011 Oden Eriksson <oeriksson@mandriva.com> 4.0.2-1mdv2010.2
- initial Mandriva package
