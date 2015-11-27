#
# configure.ac
# This file is part of dbPager Classes Library (DCL)
#
# Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
#
# DCL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# DCL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DCL; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor,
# Boston, MA  02110-1301  USA
#

# pre-initialization
m4_define([V_MAJOR], 0)
m4_define([V_MINOR], 2)
m4_define([V_REVISION], 0)

# autoconf initialization
AC_INIT([libdcl], [V_MAJOR.V_MINOR.V_REVISION], [wolfsoft@mail.ru])
AC_PREREQ([2.61])
AC_CONFIG_AUX_DIR(build)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# define product version
VERSION_MAJOR=V_MAJOR
VERSION_MINOR=V_MINOR
VERSION_REVISION=V_REVISION
AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_REVISION)

# define ABI version as current:revision:age
#
# current
#  The most recent interface number that this library implements. 
# revision
#  The implementation number of the current interface. 
# age
#  The difference between the newest and oldest interfaces that
#  this library implements. In other words, the library
#  implements all the interface numbers in the range from
#  number current - age to current. 
#
# 1. Start with version information of 0:0:0 for each libtool library.
# 2. Update the version information only immediately before a public
#    release of your software. More frequent updates are unnecessary,
#    and only guarantee that the current interface number gets larger faster.
# 3. If the library source code has changed at all since the last
#    update, then increment revision (c:r:a becomes c:r+1:a).
# 4. If any interfaces have been added, removed, or changed since
#    the last update, increment current, and set revision to 0.
# 5. If any interfaces have been added since the last public
#    release, then increment age.
# 6. If any interfaces have been removed since the last public
#    release, then set age to 0.
#
ABI_CUR=6
ABI_REV=0
ABI_AGE=5
AC_SUBST(ABI_CUR)
AC_SUBST(ABI_REV)
AC_SUBST(ABI_AGE)

# initialize doxygen stuff
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([libdcl], [doxygen.conf])

# automake initialization
AM_INIT_AUTOMAKE

# initialize gettext
AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION([0.17])

# check for programs
AC_ISC_POSIX
AC_PROG_CXX
AC_PROG_SED
LT_PROG_RC

# libtool initialization
LT_PREREQ([2.2])
LT_INIT([dlopen win32-dll])
AC_SUBST([LIBTOOL_DEPS])

# check for required tools to build the software documentation
AC_PATH_PROG([xsltproc], [xsltproc])

if test -z $xsltproc; then
	AC_MSG_ERROR([xsltproc was not found. You should install xsltproc program\
		to compile the documentation.])
fi

AC_PATH_PROG([sed], [sed])

if test -z $sed; then
	AC_MSG_ERROR([sed was not found. You should install sed program\
		to compile the documentation.])
fi

#check c++ features
AX_SHARED_PTR

#
# define build options
#

# debugging support
AC_MSG_CHECKING([whether to build with debug information])

debugger=no
AC_ARG_ENABLE(debug,
	AS_HELP_STRING(
		[--enable-debug],
		[enable debug data generation (default=no)]
	),
	debugger="$enableval"
)

AC_MSG_RESULT($debugger)

if test "$debugger" = "yes"; then
	AC_DEFINE([DEBUG], [1], [enable debug data generation])
	CXXFLAGS="$CXXFLAGS -g -fno-inline -O0"
fi

# setup variables and build flags depending on host
case "${host}" in
	*-*-cygwin* | *-*-mingw*)
		have_windows_system=yes
		;;
	*-*-freebsd*)
		CPPFLAGS="$CPPFLAGS -I/usr/include -I/usr/local/include"
		LDFLAGS="$LDFLAGS -L/usr/lib -L/usr/local/lib"
		;;
	*-*-hpux*)
		if test -z "$GCC"; then
			CFLAGS="$CFLAGS -Ae -D_HPUX_SOURCE"
		fi
		;;
	*-dec-osf4*)
		if test -z "$GCC"; then
			# Suppress all warnings
			# to get rid of the unsigned/signed char mismatch warnings.
			CFLAGS="$CFLAGS -w"
		fi
		;;
	*)
	;;
esac

if test "$have_windows_system" = yes; then
	AC_DEFINE(HAVE_WINDOWS_SYSTEM, 1, [Defined if we run on a WinAPI based system])
fi

AM_CONDITIONAL(HAVE_WINDOWS_SYSTEM, test "$have_windows_system" = yes)

# check for WinAPI libraries
AC_ARG_ENABLE(winapi,
	AS_HELP_STRING([--enable-winapi], [build Win32 GUI backend (default=detect)]),
	[use_winapi=$enableval],
	[use_winapi=auto]
)

if test "$use_winapi" != "no"; then

AC_CHECK_HEADERS([windows.h],
	[use_winapi=yes],
	[
		if test "$use_winapi" = "yes"; then
			AC_MSG_ERROR([Cannot find Win32 libraries])
		fi
		use_winapi=no;
	]
)

fi

AM_CONDITIONAL(WITH_WINAPI, test "$use_winapi" = "yes")

# check for system headers available
AC_CHECK_HEADERS([getopt.h glob.h])

# check for system functions available
AC_CHECK_FUNCS(daemon)
AC_CHECK_FUNCS([inet_ntop inet_pton])

# check for dynamic load library
save_LIBS=$LIBS
AC_SEARCH_LIBS(dlopen,dl,[
	AC_CHECK_HEADERS([dlfcn.h],[
		if test "$ac_cv_search_dlopen" != no; then
			test "$ac_cv_search_dlopen" = "none required" || DL_LIB="$ac_cv_search_dlopen"
		fi
		AC_DEFINE([HAVE_LIBDL], [1], [libdl])
		AC_DEFINE([HAVE_DLFCN_H], [1])
	])
])
LIBS=$save_LIBS
AC_SUBST(DL_LIB)

# check for thread library available
AX_PTHREAD()

# after this checks we should be able to link socket-based programs
knownsocklibs="socket net xnet nsl_s nsl inet ws2_32 wsock32 winsock"
AC_SEARCH_LIBS(gethostbyname, $knownsocklibs)
AC_SEARCH_LIBS(getservbyname, $knownsocklibs)
socklibok=no
AC_SEARCH_LIBS(select, $knownsocklibs, [socklibok=yes])
for l in ws2_32 wsock32 winsock ; do
	if test "${socklibok}" != "yes"; then
		AC_CHECK_LIB($l,main,LIBS="$LIBS -l$l";[socklibok=yes])
	fi
done
if test "${socklibok}" != "yes"; then
	AC_MSG_ERROR([
Could not figure out how to link a simple sockets-based program. Please read
the config.log file for more clues as to why this failed.
])
fi

# check for apache development files
AX_WITH_APACHE

# check for odbc development files
AX_CHECK_ODBC

# check for SSL library
AC_ARG_WITH(openssl,
	AS_HELP_STRING([--with-openssl], [OpenSSL backend]),
	[use_openssl="$withval"],
	[use_openssl="auto"]
)

AC_ARG_WITH(gnutls,
	AS_HELP_STRING([--with-gnutls], [GNU TLS backend]),
	[use_gnutls="$withval"],
	[use_gnutls="auto"]
)

ssl_ok=no;

PKG_CHECK_MODULES(OPENSSL, [openssl >= 0.9.0],
	[
		if test "$use_openssl" != "no"; then
			AC_DEFINE([HAVE_OPENSSL],[1],[Use OpenSSL library])
			ssl_ok=yes;
			use_openssl=yes;
		fi
	],
	[
		if test "$use_openssl" = "yes"; then
			AC_MSG_ERROR([Could not find OpenSSL library required.])
		fi
		use_openssl=no;
	]
)

AM_CONDITIONAL(HAVE_OPENSSL, test "$use_openssl" = "yes")

PKG_CHECK_MODULES(GNUTLS, [gnutls >= 2.0.0],
	[
		if test "$use_openssl" = "no"; then
			if test "$use_gnutls" != "no"; then
				AC_DEFINE([HAVE_GNUTLS],[1],[Use GNU TLS library])
				ssl_ok=yes;
				use_gnutls=yes;
			fi
		fi
	],
	[
		if test "$use_gnutls" = "yes"; then
			AC_MSG_ERROR([Could not find GNU TLS library required.])
		fi
		use_gnutls=no;
	]
)

if test "$ssl_ok" = "no"; then
	AC_MSG_ERROR([Could not find any of SSL libraries required (OpenSSL, GNU TLS).])
fi

AM_CONDITIONAL(HAVE_GNUTLS, test "$use_gnutls" = "yes")

# check for GTK libraries
AC_ARG_ENABLE(gtk,
	AS_HELP_STRING([--enable-gtk],[build GTK GUI backend (default=detect)]),
	[use_gtk=$enableval],
	[use_gtk=auto]
)

if test "$use_gtk" != "no"; then

PKG_CHECK_MODULES(GTK, [glib-2.0 >= 2.6.0 gtk+-2.0 >= 2.6.0 gthread-2.0 pango],
	[use_gtk=yes],
	[
		if test "$use_gtk" = "yes"; then
			AC_MSG_ERROR([Cannot find GTK2 libraries version 2.6 or later])
		fi
		use_gtk=no;
	]
)

fi

AM_CONDITIONAL(WITH_GTK, test "$use_gtk" = "yes")

# check for QT libraries
AC_ARG_ENABLE(qt,
	AS_HELP_STRING([--enable-qt],[build QT GUI backend (default=detect)]),
	[use_qt=$enableval],
	[use_qt=auto]
)

if test "$use_qt" != "no"; then

PKG_CHECK_MODULES(QT, [qt-mt >= 4.1.0],
	[use_qt=yes],
	[
		if test "$use_qt" = "yes"; then
			AC_MSG_ERROR([Cannot find QT4 library version 4.1 or later])
		fi
		use_qt=no;
	]
)

fi

AM_CONDITIONAL(WITH_QT, test "$use_qt" = "yes")

# generate makefiles
AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	m4/Makefile
	intl/Makefile
	po/Makefile.in
	include/Makefile
	include/dcl/Makefile
	src/Makefile
	src/dcl/Makefile
	src/dcl/libdclbase.rc
	src/dcl/libdclnet.rc
	src/dcl/libdclodbc.rc
	src/dcl-gtk/Makefile
	tests/Makefile
	examples/Makefile
	libdclbase.pc
	libdclnet.pc
	libdclodbc.pc
])
AC_OUTPUT
