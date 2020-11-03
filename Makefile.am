bin_PROGRAMS=ip2location

ip2location_SOURCES=ip2location.c libIP2Location/IP2Location.c
ip2location_LDADD=-lrt
ip2location_CFLAGS=-IlibIP2Location -Wall

dist_man_MANS=ip2location.1

AM_CPPFLAGS = -Wall
SUBDIRS = libIP2Location test $(NULL)