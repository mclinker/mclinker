#                     The MCLinker project
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.

aclocal -I ${MCLINKERTOP}/m4
libtoolize --force
automake --add-missing
autoconf

