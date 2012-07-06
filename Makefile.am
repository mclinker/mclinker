ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_UNITTEST
SUBDIRS = utils debug test
else
SUBDIRS = optimized test
endif

EXTRA_DIST = ./docs/MCLinker.dia
