# Include common build rules
include $(top_srcdir)/Makefile.rules

if WITH_GTK
lib_LTLIBRARIES = libdclgtk.la

libdclgtk_la_LDFLAGS = -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdclgtk_la_LIBADD = @GTK_LIBS@ @top_builddir@/src/dcl/libdclbase.la
libdclgtk_la_SOURCES = \
	gtk_objects_factory.cpp \
	gtk_application.cpp \
	gtk_dialogs.cpp

AM_CXXFLAGS += @GTK_CFLAGS@

EXTRA_DIST = *.h

endif
