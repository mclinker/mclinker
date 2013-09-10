#!/bin/sh
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

die() {
  echo "$@" 1>&2
  exit 1
}

clean() {
  echo $1 | sed -e 's/\\//g'
}

### NOTE: ############################################################
### These variables specify the tool versions we want to use.
### Periods should be escaped with backslash for use by grep.
###
### If you update these, please also update docs/GettingStarted.rst
want_autoconf_version='2\.69'
want_autoheader_version=$want_autoconf_version
want_aclocal_version='1\.11\.6'
want_libtool_version='2\.4\.2'
### END NOTE #########################################################

outfile=configure
configfile=configure.ac

want_autoconf_version_clean=$(clean $want_autoconf_version)
want_autoheader_version_clean=$(clean $want_autoheader_version)
want_aclocal_version_clean=$(clean $want_aclocal_version)
want_libtool_version_clean=$(clean $want_libtool_version)

autoconf --version | grep $want_autoconf_version > /dev/null
test $? -eq 0 || die "Your autoconf was not detected as being $want_autoconf_version_clean"
aclocal --version | grep '^aclocal.*'$want_aclocal_version > /dev/null
test $? -eq 0 || die "Your aclocal was not detected as being $want_aclocal_version_clean"
autoheader --version | grep '^autoheader.*'$want_autoheader_version > /dev/null
test $? -eq 0 || die "Your autoheader was not detected as being $want_autoheader_version_clean"
libtool --version | grep $want_libtool_version > /dev/null
test $? -eq 0 || die "Your libtool was not detected as being $want_libtool_version_clean"
echo ""
echo "### NOTE: ############################################################"
echo "### If you get *any* warnings from autoconf below you MUST fix the"
echo "### scripts in the m4 directory because there are future forward"
echo "### compatibility or platform support issues at risk. Please do NOT"
echo "### commit any configure script that was generated with warnings"
echo "### present. You should get just three 'Regenerating..' lines."
echo "######################################################################"
echo ""
echo "Regenerating aclocal.m4 with aclocal $want_aclocal_version_clean"

LIBTOOLIZE=libtoolize
my_uname=`uname`

if [ ${my_uname} = "Darwin" ]; then
  LIBTOOLIZE=glibtoolize
fi
root=`dirname $0`
cd $root

aclocal -I ./m4 || die "aclocal failed"
autoheader || die "autoheader failed"
${LIBTOOLIZE} --force || die "LIBTOOLIZE failed"
automake --add-missing || die "automake failed"
autoconf || die "autoconf failed"
exit 0
