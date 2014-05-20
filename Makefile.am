ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_OPTIMIZED
SUBDIRS = utils optimized test tools include
else
SUBDIRS = utils debug test tools include
endif

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh
