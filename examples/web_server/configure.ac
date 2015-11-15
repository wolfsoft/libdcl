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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DCL; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor,
# Boston, MA  02110-1301  USA
#

# initialization
AC_INIT([web_server], [1.0.0], [wolfsoft@mail.ru])
AC_PREREQ([2.61])
AC_CONFIG_AUX_DIR(build)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# automake initialization
AM_INIT_AUTOMAKE

# initialize gettext
AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

# check for programs
AC_ISC_POSIX
AC_PROG_CXX

# libtool initialization
LT_PREREQ([2.2])
LT_INIT([dlopen win32-dll])
AC_SUBST([LIBTOOL_DEPS])

#
# define build options
#

# debugging support
AC_MSG_CHECKING([whether to build with debug information])

AC_ARG_ENABLE(debug,
	AS_HELP_STRING(
		[--enable-debug],
		[enable debug data generation (default=no)]),
	[debugger="$enableval"],
	[debugger=no]
)

AC_MSG_RESULT($debugger)

if test "$debugger" = "yes"; then
	AC_DEFINE([DEBUG], [1], [enable debug data generation])
	CXXFLAGS="$CXXFLAGS -g -fno-inline"
fi

# Checks for libraries
PKG_CHECK_MODULES(libdclbase, [libdclbase])
PKG_CHECK_MODULES(libdclnet, [libdclnet])

# Generate makefiles
AC_CONFIG_FILES([
	Makefile
	m4/Makefile
	po/Makefile.in
	src/Makefile
])

# Run
AC_OUTPUT

