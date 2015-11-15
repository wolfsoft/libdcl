# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_APACHE
noinst_LTLIBRARIES = mod_cpphello.la

mod_cpphello_la_LDFLAGS = -Wl,--as-needed -avoid-version -module -rpath /fake
mod_cpphello_la_LIBADD = \
	@APACHE_LIBS@ \
	@top_builddir@/src/dcl/libdclbase.la \
	@top_builddir@/src/dcl/libdclnet.la

mod_cpphello_la_SOURCES = main.cpp

AM_CXXFLAGS += @APACHE_CFLAGS@

endif
