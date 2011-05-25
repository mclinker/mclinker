
TOPLEVEL=..
SRCDIR=${TOPLEVEL}/src

bin_PROGRAMS = bold

if ENABLE_UNITTEST
bold_CPPFLAGS = -DENABLE_UNITTEST -DDEBUG -I$(srcdir)/../utils/gtest/include
bold_LDADD = -g -O1 -lgtest -L$(srcdir)/../utils/gtest/lib/
else
bold_CPPFLAGS = -g -O1 -DDEBUG -I$(srcdir)/../utils/gtest/include
bold_LDADD =
endif

bold_SOURCES = ${SRCDIR}/main.cpp

