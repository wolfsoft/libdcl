bin_PROGRAMS = web_server

web_server_SOURCES = main.cpp www_server.cpp
web_server_LDADD = \
	@libdclbase_LIBS@ \
	@libdclnet_LIBS@
web_server_CPPFLAGS = \
	@libdclbase_CFLAGS@ \
	@libdclnet_CFLAGS@

EXTRA_DIST = *.h

