AM_CFLAGS = -Wall -std=gnu99 
SUBDIRS = libae test
ACLOCAL_AMFLAGS = -I m4

.PHONY: source-release

source-release:
	git archive $$(cat version.txt) \
	--prefix libae/ \
	--output ../Releases/libae-$$(cat version.txt).tar.gz

