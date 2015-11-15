# Include common build rules
include $(top_srcdir)/Makefile.rules

lib_LTLIBRARIES = libdclbase.la libdclnet.la

if WITH_ODBC
lib_LTLIBRARIES += libdclodbc.la
endif

# include support of building Win32 resource files
SUFFIXES = .rc

LTRCCOMPILE = $(LIBTOOL) --mode=compile $(RC) \
	`echo $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) | \
	sed -e 's/-I/--include-dir /g;s/-D/--define /g'`

.rc.lo:
	$(LTRCCOMPILE) -i $< -o $@
# --

if HAVE_WINDOWS_SYSTEM
libdclbase_res = libdclbase.lo libdclbase_msgs.lo
libdclbase_res_ldflag = -Wl,.libs/libdclbase.o
else
libdclbase_res =
libdclbase_res_ldflag =
endif

libdclbase_la_LDFLAGS = $(libdclbase_res_ldflag) -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdclbase_la_LIBADD = @LTLIBINTL@ @DL_LIB@ @PTHREAD_LIBS@
libdclbase_la_DEPENDENCIES = $(libdclbase_res)
libdclbase_la_SOURCES = \
	i18n.cpp \
	app_config.cpp \
	application.cpp \
	cmdline_parameters.cpp \
	datetime.cpp \
	mutex.cpp \
	rwlock.cpp \
	semaphore.cpp \
	event.cpp \
	daemon_application.cpp \
	factory.cpp \
	filefs.cpp \
	gui_application.cpp \
	plugin.cpp \
	strutils.cpp \
	thread.cpp \
	process.cpp \
	url.cpp \
	uuid.cpp \
	encoder_md5.cpp \
	codec_base64.cpp \
	mimetype.cpp

if HAVE_WINDOWS_SYSTEM
libdclnet_res = libdclnet.lo
libdclnet_res_ldflag = -Wl,.libs/libdclnet.o
else
libdclnet_res =
libdclnet_res_ldflag =
endif

libdclnet_la_LDFLAGS = $(libdclnet_res_ldflag) -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdclnet_la_LIBADD = @DL_LIB@ @APACHE_LIBTOOL@ libdclbase.la

if HAVE_OPENSSL
libdclnet_la_LIBADD += @OPENSSL_LIBS@
endif

if HAVE_GNUTLS
libdclnet_la_LIBADD += @GNUTLS_LIBS@
endif

libdclnet_la_DEPENDENCIES = $(libdclnet_res)
libdclnet_la_SOURCES = \
	http_header.cpp \
	http_content_parser.cpp \
	cgi_application.cpp \
	socket.cpp \
	socket_stream.cpp \
	ssl_socket.cpp \
	tcp_server.cpp \
	tcp_client.cpp \
	http_server.cpp

if WITH_APACHE	
libdclnet_la_SOURCES += apache_application.cpp
endif
if HAVE_WINDOWS_SYSTEM
libdclnet_la_SOURCES += isapi_application.cpp
endif

if WITH_ODBC

if HAVE_WINDOWS_SYSTEM
libdclodbc_res = libdclodbc.lo
libdclodbc_res_ldflag = -Wl,.libs/libdclodbc.o
else
libdclodbc_res =
libdclodbc_res_ldflag =
endif

libdclodbc_la_LDFLAGS = $(libdclodbc_res_ldflag) -version-info $(ABI_CUR):$(ABI_REV):$(ABI_AGE) -no-undefined
libdclodbc_la_LIBADD = @DL_LIB@ @ODBC_LIBS@ libdclbase.la
libdclodbc_la_DEPENDENCIES = $(libdclodbc_res)
libdclodbc_la_SOURCES = \
	connection.cpp \
	query.cpp
endif

AM_CXXFLAGS += @APACHE_CFLAGS@ @ODBC_CFLAGS@ @PTHREAD_CFLAGS@

if HAVE_OPENSSL
AM_CXXFLAGS += @OPENSSL_CFLAGS@
endif

if HAVE_GNUTLS
AM_CXXFLAGS += @GNUTLS_CFLAGS@
endif

EXTRA_DIST = posix/* win32/* \
	openssl_socket.cpp gnutls_socket.cpp \
	libdclbase_msgs.*

