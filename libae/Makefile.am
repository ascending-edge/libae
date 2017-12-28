# Is there a way that I don't have to repeat libae0 a million times?
lib_LTLIBRARIES = libae0.la

libae0_la_LDFLAGS = \
	-no-undefined \
	-version $(LIBAE_MAJOR):$(LIBAE_MINOR):$(LIBAE_AGE) 

libae0_la_CPPFLAGS = \
	-I ../include/

libae0_la_SOURCES = \
	ae.c \
	event.c \
	log.c \
	res.c \
	pool.c \
	ptrarray.c \
	str.c \
	buf.c

include_HEADERS = \
	../include/ae/ae.h \
	../include/ae/types.h \
	../include/ae/buf.h \
	../include/ae/log.h \
	../include/ae/pool.h \
	../include/ae/ptrarray.h \
	../include/ae/res.h \
	../include/ae/try.h \
	../include/ae/misc.h \
	../include/ae/str.h 
