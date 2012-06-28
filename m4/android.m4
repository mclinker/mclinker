dnl
dnl @synopsis CHECK_LLVM([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl @synopsis DEFMACRO_ANDROID
dnl
dnl @summary define macros which is predefined in Android system
dnl   ALOGE
dnl
dnl Luba Tang <lubatang@gmail.com>


AC_DEFUN([DEFMACRO_ANDROID],
[dnl
	AC_DEFINE_UNQUOTED([ALOGE(fmt, args...)],
                  [printf("%s:%s:%d: "fmt, __FILE__, __FUNCTION__, __LINE__, args)],
                  [Define ALOGE for debug message])
])
