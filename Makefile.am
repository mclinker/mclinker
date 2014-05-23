ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_OPTIMIZED
MCLD = optimized
else
MCLD = debug
endif
SUBDIRS = $(MCLD) include tools utils unittests test

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh

.PHONY: unittests
unittests:
	cd unittests && $(MAKE) $(AM_MAKEFLAGS) unittests
