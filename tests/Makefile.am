# Include common build rules
include $(top_srcdir)/Makefile.rules

check_LTLIBRARIES = libplugin.la
libplugin_la_LDFLAGS = -no-undefined -avoid-version -module -rpath /fake
libplugin_la_SOURCES = plugin.cpp test_plugin.h
libplugin_la_LIBADD = @top_builddir@/src/dcl/libdclbase.la

check_PROGRAMS = \
	test_strutils \
	test_shared_ptr \
	test_plugin \
	test_config \
	test_thread \
	test_process \
	test_factory \
	test_any \
	test_url \
	test_mimetype \
	test_uuid \
	test_datetime \
	test_encode_md5 \
	test_codec_base64 \
	test_pool \
	test_http_content_parser \
	test_http_server \
	test_tcp_server

if WITH_ODBC
check_PROGRAMS += test_odbc test_pool_odbc
test_odbc_SOURCES = test_odbc.cpp
test_odbc_LDADD = @top_builddir@/src/dcl/libdclodbc.la \
	@top_builddir@/src/dcl/libdclbase.la
test_pool_odbc_SOURCES = test_pool_odbc.cpp
test_pool_odbc_LDADD = @top_builddir@/src/dcl/libdclodbc.la \
	@top_builddir@/src/dcl/libdclbase.la
endif

test_strutils_SOURCES = test_strutils.cpp
test_strutils_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_shared_ptr_SOURCES = test_shared_ptr.cpp
test_shared_ptr_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_plugin_SOURCES = test_plugin.cpp test_plugin.h
test_plugin_LDADD = @top_builddir@/src/dcl/libdclbase.la
test_plugin_DEPENDENCIES = libplugin.la

test_config_SOURCES = test_config.cpp
test_config_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_thread_SOURCES = test_thread.cpp
test_thread_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_process_SOURCES = test_process.cpp
test_process_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_factory_SOURCES = test_factory.cpp
test_factory_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_url_SOURCES = test_url.cpp
test_url_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_mimetype_SOURCES = test_mimetype.cpp
test_mimetype_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_uuid_SOURCES = test_uuid.cpp
test_uuid_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_datetime_SOURCES = test_datetime.cpp
test_datetime_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_codec_base64_SOURCES = test_codec_base64.cpp
test_codec_base64_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_encode_md5_SOURCES = test_encode_md5.cpp
test_encode_md5_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_any_SOURCES = test_any.cpp
test_any_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_pool_SOURCES = test_pool.cpp
test_pool_LDADD = @top_builddir@/src/dcl/libdclbase.la

test_tcp_server_SOURCES = test_tcp_server.cpp
test_tcp_server_LDADD = @top_builddir@/src/dcl/libdclbase.la \
	@top_builddir@/src/dcl/libdclnet.la

test_http_server_SOURCES = test_http_server.cpp
test_http_server_LDADD = @top_builddir@/src/dcl/libdclbase.la \
	@top_builddir@/src/dcl/libdclnet.la

test_http_content_parser_SOURCES = test_http_content_parser.cpp
test_http_content_parser_LDADD = @top_builddir@/src/dcl/libdclbase.la \
	@top_builddir@/src/dcl/libdclnet.la

TESTS = \
	test_strutils \
	test_shared_ptr \
	test_plugin \
	test_config \
	test_thread \
	test_process \
	test_factory \
	test_url \
	test_mimetype \
	test_uuid \
	test_datetime \
	test_encode_md5 \
	test_codec_base64 \
	test_any \
	test_pool \
	test_http_server \
	test_tcp_server

if WITH_ODBC
TESTS += test_odbc test_pool_odbc
endif

EXTRA_DIST = test.conf test_include.conf *.h
