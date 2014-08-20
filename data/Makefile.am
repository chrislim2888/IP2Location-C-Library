convert:
	@if ! test -s ./IP-COUNTRY.BIN; then \
		perl ip-country.pl; \
	fi
	@if ! test -s ./IPV6-COUNTRY.BIN; then \
		perl ipv6-country.pl; \
	fi

clean-am:
	@if test -s ./IP-COUNTRY.BIN; then \
		rm -f ./IP-COUNTRY.BIN; \
	fi
	@if test -s ./IPV6-COUNTRY.BIN; then \
		rm -f ./IPV6-COUNTRY.BIN; \
	fi

IPV6_DB_FILE = $(pkgdatadir)/IPV6-COUNTRY.BIN
IPV4_DB_FILE = $(pkgdatadir)/IP-COUNTRY.BIN

pkgdata_DATA =
dist_pkgdata_DATA = IP-COUNTRY.BIN \
		    IPV6-COUNTRY.BIN 	
DB_FILE=IP-COUNTRY.BIN

install-data-hook:
	@if test -f "$(DESTDIR)$(IPV4_DB_FILE)" ; then \
		echo "$@ DB file exists $(DESTDIR)$(IPV4_DB_FILE)" ; \
	elif [ "x$(DB_FILE)" != "x" ] ; then \
		echo "$(INSTALL_DATA) IP-COUNTRY.BIN $(DESTDIR)"; \
		$(INSTALL_DATA) "$(srcdir)/IP-COUNTRY.BIN" "$(DESTDIR)$(IPV4_DB_FILE)"; \
		$(INSTALL_DATA) "$(srcdir)/IPV6-COUNTRY.BIN" "$(DESTDIR)$(IPV6_DB_FILE)"; \
	fi

uninstall-hook:
	@if test -f "$(DESTDIR)$(IPV4_DB_FILE)" ; then \
		rm -f "$(DESTDIR)$(IPV4_DB_FILE)"; \
	fi
	@if test -f "$(DESTDIR)$(IPV6_DB_FILE)" ; then \
		rm -f "$(DESTDIR)$(IPV6_DB_FILE)"; \
	fi


