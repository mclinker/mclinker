
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src
UNITTEST=${TOPLEVEL}/unittests

bin_PROGRAMS = bold

if ENABLE_UNITTEST
bold_CPPFLAGS = -g -O1 -DENABLE_UNITTEST -DDEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include -I$(srcdir)/../unittests
bold_LDADD = -lgtest -L$(srcdir)/../utils/gtest/lib/
else
bold_CPPFLAGS = -g -O1 -DDEBUG -I$(srcdir)/../utils/gtest/include -I$(srcdir)/../include
bold_LDADD =
endif

bold_SOURCES = ${SRCDIR}/main.cpp

