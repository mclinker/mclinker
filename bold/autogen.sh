#!/bin/bash
##############################################################################
#   Copyright (C) 2011-
#     Embedded and Web Computing Lab, National Taiwan University
#     MediaTek, Inc.
#
aclocal
libtoolize --force
automake --add-missing
autoconf

