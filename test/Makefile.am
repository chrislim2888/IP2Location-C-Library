NULL =

AM_CPPFLAGS =	\
	-I$(top_srcdir)/libIP2Location		\
	-Wall -ansi				\
	$(NULL)

noinst_PROGRAMS = test-IP2Location

DEPS = $(top_builddir)/libIP2Location/libIP2Location.la
LDADDS = $(top_builddir)/libIP2Location/libIP2Location.la

test_IP2Location_SOURCES = test-IP2Location.c
test_IP2Location_LDFLAGS =
test_IP2Location_DEPENDENCIES = $(DEPS)
test_IP2Location_LDADD = $(LDADDS)

EXTRA_DIST = country_test_data.txt
TESTS = test-IP2Location
