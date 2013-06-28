AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I. -pipe -D _GNU_SOURCE
AM_CXXFLAGS = -Wall -Werror

lib_LTLIBRARIES = libIP2Location.la
include_HEADERS = IP2Location.h IP2Loc_DBInterface.c iMath/imath.h

libIP2Location_la_SOURCES = IP2Location.c \
							IP2Loc_DBInterface.c \
                            iMath/imath.c 
														
libIP2Location_la_LDFLAGS = -module -no-undefined -avoid-version

libIP2Location_la_CFLAGS = -IiMath/
