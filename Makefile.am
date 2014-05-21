ACLOCAL_AMFLAGS=-I m4

AUTOMAKE_OPTIONS = foreign

if ENABLE_OPTIMIZED
SUBDIRS = optimized test tools include utils unittests
else
SUBDIRS = debug test tools include utils unittests
endif

EXTRA_DIST = ./docs/MCLinker.dia ./autogen.sh

.PHONY: unittests
unittests:
	cd unittests && make unittests
