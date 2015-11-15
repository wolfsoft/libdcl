#
# SYNOPSIS
#
#   AX_WITH_ODBC()
#
# DESCRIPTION
#
#   Locates installed ODBC library (unixODBC, iODBC or Win32 ODBC).
#
# LAST MODIFICATION
#
#   2008-06-22
#
# COPYRIGHT
#
#   Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
#
#   This is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation version 3.
#
#   This software is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with dbPager Server; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor,
#   Boston, MA  02110-1301  USA

AC_DEFUN([AX_CHECK_ODBC],
[

# providing the options
AC_ARG_WITH(odbc,
	AS_HELP_STRING([--with-odbc], [ODBC support (default=detect)]),
	[use_odbc="$withval"],
	[use_odbc="auto"]
)

AC_ARG_WITH(odbc-includes,
	AS_HELP_STRING([--with-odbc-includes[=DIR]],[Use ODBC headers from DIR]),
	[odbc_includes_dir=$withval],
	[odbc_includes_dir=""]
)

AC_ARG_WITH(odbc-libraries,
	AS_HELP_STRING([--with-odbc-libraries[=DIR]],[Find ODBC libraries in DIR]),
	[odbc_libraries_dir=$withval],
	[odbc_libraries_dir=""]
)

AC_ARG_WITH(odbc-library,
	AS_HELP_STRING([--with-odbc-backend[=LIB]],[Use the ODBC backend library by LIB]),
	[odbc_library_name=$withval],
	[odbc_library_name=""]
)

# searching for the libraries available
if test "$use_odbc" != "no"; then

SAVE_CFLAGS=$CFLAGS;
SAVE_LIBS=$LIBS;

if test "x$odbc_includes_dir" != "x"; then
	CFLAGS="-I$odbc_includes_dir $CFLAGS";
fi

if test "x$odbc_libraries_dir" != "x"; then
	LIBS="-L$odbc_libraries_dir $LIBS";
fi

AC_CHECK_HEADERS([sql.h sqlext.h],
	[odbc_ok=maybe],
	[
		odbc_ok=no;
		if test "$use_odbc" = "yes"; then
			AC_MSG_ERROR(
[The ODBC library headers are not found.
Please install devel ODBC library package if it is not installed
or specify --with-odbc-includes option with directory
where ODBC library headers are installed to.])
		fi
	],
[#ifdef WIN32
#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>]
)

if test "$odbc_ok" = "maybe"; then
	if test "x$odbc_library_name" != "x"; then
		S_LIBS=$LIBS;
		LIBS="$LIBS -l$odbc_library_name";
		AC_CHECK_LIB($odbc_library_name, SQLConnect, [odbc_ok=yes], [odbc_ok=maybe])
		LIBS=$S_LIBS;
	fi
fi

if test "$odbc_ok" = "maybe"; then
	odbc_library_name=odbc
	S_LIBS=$LIBS;
	LIBS="$LIBS -l$odbc_library_name";
	AC_CHECK_LIB($odbc_library_name, SQLConnect, [odbc_ok=yes], [odbc_ok=maybe])
	LIBS=$S_LIBS;
fi

if test "$odbc_ok" = "maybe"; then
	odbc_library_name=iodbc
	S_LIBS=$LIBS;
	LIBS="$LIBS -l$odbc_library_name";
	AC_CHECK_LIB($odbc_library_name, SQLConnect, [odbc_ok=yes], [odbc_ok=maybe])
	LIBS=$S_LIBS;
fi

if test "$odbc_ok" = "maybe"; then
	odbc_library_name=odbc32
	S_LIBS=$LIBS;
	LIBS="$LIBS -l$odbc_library_name";
	AC_MSG_CHECKING([for SQLConnect in -lodbc32])
	AC_TRY_LINK(
[#include <windows.h>
#include <sql.h>],
[int main() { return 0; }
int t() { void ((*volatile p)()); p = (void((*)()))SQLConnect; return 0; }],
		[
			odbc_ok=yes
			AC_MSG_RESULT(yes)
		],
		[
			odbc_ok=no;
			AC_MSG_RESULT(no)
			if test "$use_odbc" = "yes"; then
				AC_MSG_ERROR(
[The ODBC library is not found.
Please install development ODBC library package if it is not installed
or specify --with-odbc-libraries option with directory
where ODBC library is installed to.])
			fi
		])
	LIBS=$S_LIBS;
fi

if test "$odbc_ok" = "yes"; then
	if test "x$odbc_includes_dir" != "x"; then
		ODBC_CFLAGS="-I$odbc_includes_dir";
	fi
	if test "x$odbc_libraries_dir" != "x"; then
		ODBC_LIBS="-L$odbc_libraries_dir";
	fi
	ODBC_LIBS="$ODBC_LIBS -l$odbc_library_name";
	AC_DEFINE(HAVE_LIBODBC,,defined if using ODBC library)
	AC_DEFINE(HAVE_SQL_H,,)
	AC_DEFINE(HAVE_SQLEXT_H,,)
fi

AM_CONDITIONAL(WITH_ODBC, test "$odbc_ok" = "yes")
AC_SUBST(ODBC_CFLAGS)
AC_SUBST(ODBC_LIBS)

CFLAGS=$SAVE_CFLAGS;
LIBS=$SAVE_LIBS;

fi
])

