AC_PREREQ([2.63])
AC_INIT([dawati-artwork],
        [0.90.1],
        [http://www.dawati.com])

AC_CONFIG_SRCDIR([sound/index.theme])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])
AM_SILENT_RULES([yes])

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_PROG_LN_S
AC_SUBST(VERSION)

dnl Initialize libtool
AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG()

PKG_CHECK_MODULES(GTK,
		  [gtk+-2.0 >= 2.8.0],,
		  [AC_MSG_ERROR([GTK+ 2.8 required for the gtk2 engine])])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

DEVELOPMENT_CFLAGS="-Wall"
AC_SUBST(DEVELOPMENT_CFLAGS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
AC_SUBST(GTK_VERSION)

AC_CONFIG_FILES([
  Makefile
  backgrounds/Makefile
  cursor/Makefile
  icons/Makefile
  icons/24x24/Makefile
  icons/48x48/Makefile
  sound/Makefile
  sound/stereo/Makefile
  ui/Makefile
  ui/engine/Makefile
])
AC_OUTPUT
