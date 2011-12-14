#!/bin/sh
#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

root=`dirname $0`
cd $root

aclocal -I ./m4
libtoolize --force
automake --add-missing
autoconf
