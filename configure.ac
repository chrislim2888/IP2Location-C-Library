AC_PREREQ(2.59)
AC_INIT([libIP2Location], [2.1.5], [support@ip2location.com])
AM_INIT_AUTOMAKE([libIP2Location], [2.1.5])

AC_PREFIX_DEFAULT(/usr/local)
AM_CONFIG_HEADER([config.h])
# AM_MAINTAINER_MODE

AM_CONDITIONAL(NDEBUG, test "$disable_debug" = yes)
AM_CONDITIONAL(PROFILE, test "$enable_profile" = yes)

AC_CANONICAL_HOST
AC_C_CONST
AC_C_BIGENDIAN

dnl Check for some target-specific stuff
case "$host" in
*-*-freebsd*)
  # FreeBSD forgot to  install magic.h in a sane location. Bug or Feature?
  CPPFLAGS="$CPPFLAGS -I/usr/local/include -I/usr/src/contrib/file/"
  LDFLAGS="$LDFLAGS -L/usr/local/lib -R/usr/local/lib"
  CFLAGS="$CFLAGS -lrt"
  ;;
*-*-linux*)
  CPPFLAGS="$CPPFLAGS -D _GNU_SOURCE -I/usr/local/include"
  LDFLAGS="$LDFLAGS -L/usr/local/lib -R/usr/local/lib"
  CFLAGS="$CFLAGS -lrt"
  ;;
*-*-darwin*)
  CPPFLAGS="$CPPFLAGS -I/opt/local/include"
  LDFLAGS="$LDFLAGS -L/opt/local/lib"
  if test "$GCC" = "yes"; then
          CFLAGS="$CFLAGS -no-cpp-precomp"
  fi
  ;;
esac

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for header files.
#AC_HEADER_DIRENT
#AC_HEADER_STDC
#AC_HEADER_SYS_WAIT
#AC_HEADER_TIME
#AC_HEADER_STDBOOL

AC_CHECK_HEADERS([netinet/in.h stdlib.h string.h unistd.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UID_T
AC_STRUCT_TM
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL

AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(off_t)


AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

AC_DEFINE_DIR(PREFIX, prefix, [default working directory])
AC_DEFINE_DIR(LOCALESTATEDIR, localstatedir, [where to put logs etc])
AC_DEFINE_DIR(LIBDIR, libdir, [where to look for plugins])
AC_DEFINE_DIR(SYSCONFDIR, [sysconfdir], [System configuration dir])

AC_CONFIG_FILES([Makefile
                 libIP2Location/Makefile
                 test/Makefile])
                 
AC_OUTPUT
