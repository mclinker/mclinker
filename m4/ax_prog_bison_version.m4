# SYNOPSIS
#
#   AX_PROG_BISON_VERSION([VERSION],[ACTION-IF-TRUE],[ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   Makes sure that bison supports the version indicated. If true the shell
#   commands in ACTION-IF-TRUE are executed. If not the shell commands in
#   ACTION-IF-FALSE are run. Note if $BISON is not set (for example by
#   running AC_CHECK_PROG or AC_PATH_PROG) the macro will fail.
#
#   Example:
#
#     AC_PATH_PROG([BISON],[bison])
#     AX_PROG_FLEX_VERSION([2.5.4],[ ... ],[ ... ])
#
#   This will check to make sure that the bison you have supports at least
#   version 2.5.4.
#
#   NOTE: This macro uses the $BISON variable to perform the check.
#   AX_WITH_BISON can be used to set that variable prior to running this
#   macro. The $BISON_VERSION variable will be valorized with the detected
#   version.
#
#   NOTE: This macro is based upon the original AX_PROG_PYTHON_VERSION macro
#   from Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
# LICENSE
#
#   Copyright (c) 2013 Pete Chou <petechou@gmail.com>
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_PROG_BISON_VERSION],[
    AC_REQUIRE([AX_PROG_BISON])
    AC_REQUIRE([AC_PROG_SED])
    AC_REQUIRE([AC_PROG_GREP])

    AS_IF([test -n "$BISON"],[
        ax_bison_version="$1"

        AC_MSG_CHECKING([for bison version])
        changequote(<<,>>)
        bison_version=`$BISON --version 2>&1 | $GREP "^bison " | $SED -e 's/^.* \([0-9]*\(\.[0-9]*\)\{1,2\}\)/\1/'`
        changequote([,])
        AC_MSG_RESULT($bison_version)

	AC_SUBST([BISON_VERSION],[$bison_version])

        AX_COMPARE_VERSION([$ax_bison_version],[le],[$bison_version],[
	    :
            $2
        ],[
	    :
            $3
        ])
    ],[
        AC_MSG_WARN([could not find the bison parser generator])
        $3
    ])
])
