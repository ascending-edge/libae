# Is there a way that I don't have to repeat libae0 a million times?
lib_LTLIBRARIES = libae0.la

libae0_la_LDFLAGS = \
	-no-undefined \
	-version $(LIBAE_MAJOR):$(LIBAE_MINOR):$(LIBAE_AGE) 

libae0_la_CPPFLAGS = \
	-I ../include/

libae0_la_SOURCES = \
	ae.c \
	buf.c \
	event.c \
	log.c \
	opt.c \
	pool.c \
	ptrarray.c \
	res.c \
	str.c \
	time.c 
headerdir = $(prefix)/include/ae
header_DATA = \
	../include/ae/ae.h \
	../include/ae/buf.h \
	../include/ae/event.h \
	../include/ae/log.h \
	../include/ae/misc.h \
	../include/ae/opt.h \
	../include/ae/pool.h \
	../include/ae/ptrarray.h \
	../include/ae/res.h \
	../include/ae/str.h \
	../include/ae/time.h
	../include/ae/try.h \
	../include/ae/types.h 

