AC_INIT(cygwin_crash_reporter, 0.1)
AM_INIT_AUTOMAKE(foreign no-dist-gzip dist-xz -Wall)
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AC_CANONICAL_HOST
AC_PROG_CXX
AC_CHECK_TOOL(WINDRES, windres)

case $host in
  *-*-cygwin* )
    AC_MSG_ERROR([cygwin_crash_reporter must be cross-compiled])
    ;;
  *-*-mingw* )
    ;;
  * )
    AC_MSG_ERROR([cygwin_crash_reporter only works on Windows targets])
    ;;
esac

CXXFLAGS="-g -O0 -Wall -Wextra $CFLAGS"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
