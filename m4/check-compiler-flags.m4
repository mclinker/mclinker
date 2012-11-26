dnl
dnl @synopsis CHECK_COMPILER_FLAG
dnl
dnl @summary Check compiler's flags. Add the flags if supported.
dnl
dnl Luba Tang <lubatang@gmail.com>
AC_DEFUN([CHECK_COMPILER_FLAG],
[dnl
  AS_VAR_PUSHDEF([SAVEDVARS],[ac_cv_check_[]_AC_LANG_ABBREV[]_flags_$1])

  AC_CACHE_CHECK(
    [if _AC_LANG compiler supports $1],
    SAVEDVARS,
    [saved_flags=$[]_AC_LANG_PREFIX[]FLAGS
    _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $1"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
      [AS_VAR_SET(SAVEDVARS,[yes])],
      [AS_VAR_SET(SAVEDVARS,[no])])
    _AC_LANG_PREFIX[]FLAGS=$saved_flags])

  AS_IF([test x"AS_VAR_GET(SAVEDVARS)" = xyes],
    [_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $1"],
    [])

  AS_VAR_POPDEF([SAVEDVARS])
])

