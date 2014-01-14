ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_UNITTEST
SUBDIRS = utils debug test tools include
else
SUBDIRS = optimized tools include
endif

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh
