AC_INIT(cygwin_crash_reporter, 0.1)

AM_INIT_AUTOMAKE(foreign no-dist-gzip dist-xz -Wall)
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])
AM_PROG_AR

AC_CANONICAL_HOST
AC_PROG_CXX
AC_PROG_RANLIB
AC_CHECK_TOOL(WINDRES, windres)

case $host in
  *-*-cygwin* )
    AC_MSG_ERROR([cygwin_crash_reporter should be built as a native Windows application])
    ;;
  *-*-mingw* )
    ;;
  * )
    AC_MSG_ERROR([cygwin_crash_reporter only works on Windows targets])
    ;;
esac

CXXFLAGS="-g -O0 -Wall -Wextra $CXXFLAGS"

AC_CONFIG_FILES([Makefile cygwin_crash_reporter.man])
AC_OUTPUT
