ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_UNITTEST
SUBDIRS = utils optimized debug test
else
SUBDIRS = optimized debug
endif

EXTRA_DIST = ./docs/MCLinker.dia
