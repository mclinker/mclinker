# SYNOPSIS
#
#   AX_PROG_FLEX_VERSION([VERSION],[ACTION-IF-TRUE],[ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   Makes sure that flex supports the version indicated. If true the shell
#   commands in ACTION-IF-TRUE are executed. If not the shell commands in
#   ACTION-IF-FALSE are run. Note if $FLEX is not set (for example by
#   running AC_CHECK_PROG or AC_PATH_PROG) the macro will fail.
#
#   Example:
#
#     AC_PATH_PROG([FLEX],[flex])
#     AX_PROG_FLEX_VERSION([2.5.35],[ ... ],[ ... ])
#
#   This will check to make sure that the flex you have supports at least
#   version 2.5.35.
#
#   NOTE: This macro uses the $FLEX variable to perform the check.
#   AX_WITH_FLEX can be used to set that variable prior to running this
#   macro. The $FLEX_VERSION variable will be valorized with the detected
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

AC_DEFUN([AX_PROG_FLEX_VERSION],[
    AC_REQUIRE([AX_PROG_FLEX])
    AC_REQUIRE([AC_PROG_SED])
    AC_REQUIRE([AC_PROG_GREP])

    AS_IF([test -n "$FLEX"],[
        ax_flex_version="$1"

        AC_MSG_CHECKING([for flex version])
        changequote(<<,>>)
        flex_version=`$FLEX --version 2>&1 | $GREP "^flex " | $SED -e 's/^.* \([0-9]*\.[0-9]*\.[0-9]*\)/\1/'`
        changequote([,])
        AC_MSG_RESULT($flex_version)

	AC_SUBST([FLEX_VERSION],[$flex_version])

        AX_COMPARE_VERSION([$ax_flex_version],[le],[$flex_version],[
	    :
            $2
        ],[
	    :
            $3
        ])
    ],[
        AC_MSG_WARN([could not find the flex lexer generator])
        $3
    ])
])
