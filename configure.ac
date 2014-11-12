AC_INIT([cygwin-crashreporter], m4_esyscmd([cat version | tr -d '\n']))

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
    AC_MSG_ERROR([cygwin-crashreporter should be built as a native Windows application])
    ;;
  *-*-mingw* )
    ;;
  * )
    AC_MSG_ERROR([cygwin-crashreporter only works on Windows targets])
    ;;
esac

CXXFLAGS="-Wall -Wextra -D_FORTIFY_SOURCE=2 $CXXFLAGS"

AC_CHECK_LIB([wininet], [main], [], AC_MSG_ERROR([libwininet is required]))
AC_CHECK_LIB([breakpad_client], [main], [], AC_MSG_ERROR([libbreakpad_client is required]))

AC_CONFIG_FILES([Makefile cygwin-crashreporter.man cygwin-crashreporter-server.man])
AC_OUTPUT
