ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_UNITTEST
SUBDIRS = utils debug test
else
SUBDIRS = optimized
endif

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh
