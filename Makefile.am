# General automake options.
ACLOCAL_AMFLAGS = -I m4

# Include common build rules
include $(top_srcdir)/Makefile.rules

# The list of subdirectories containing Makefiles.
SUBDIRS = intl doc examples m4 po include src tests

# Additional files to be distributed.
EXTRA_DIST = build debian libdcl.spec examples gettext.h autogen.sh autoclean.sh

# Install pkg-config files
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	libdclbase.pc \
	libdclnet.pc \
	libdclodbc.pc
