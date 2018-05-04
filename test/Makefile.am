noinst_PROGRAMS = ae-test server
ae_test_DEPENDENCIES = ../libae/.libs/libae0.la
ae_test_LDADD = ../libae/.libs/libae0.la -lm -lpthread -lrt
ae_test_CPPFLAGS = \
	-I ../include
ae_test_SOURCES = \
	ae-test.c


server_DEPENDENCIES = ../libae/.libs/libae0.la
server_LDADD = ../libae/.libs/libae0.la -lm -lpthread -lrt
server_CPPFLAGS = \
	-I ../include
server_SOURCES = \
	server.c
