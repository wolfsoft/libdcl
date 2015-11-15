# Include common build rules
include $(top_srcdir)/Makefile.rules

if HAVE_WINDOWS_SYSTEM
noinst_LTLIBRARIES = cpphello.la

cpphello_la_LDFLAGS = -Wl,--add-stdcall-alias -no-undefined -avoid-version -module -rpath /fake
cpphello_la_LIBADD = \
	@top_builddir@/src/dcl/libdclbase.la \
	@top_builddir@/src/dcl/libdclnet.la

cpphello_la_SOURCES = main.cpp

endif
