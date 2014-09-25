ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

SUBDIRS = include lib tools utils unittests test

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh

.PHONY: unittests
unittests:
	cd unittests && $(MAKE) $(AM_MAKEFLAGS) unittests

include Makefile.am.cpplint
