ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_UNITTEST
SUBDIRS = utils debug test tools
else
SUBDIRS = optimized tools
endif

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh
