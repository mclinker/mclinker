; RUN: %MCLinker -mtriple=arm-linux-gnueabi -L%p/../libs/ARM/Android/android-14/ -lc -o %t.arm.so
; RUN: test -f %t.arm.so
; RUN: %MCLinker -mtriple=i386-linux-gnueabi -L%p/../libs/X86/Android/android-14/ -lc -o %t.i386.so
; RUN: test -f %t.i386.so
; RUN: %MCLinker -mtriple=mipsel-linux-gnueabi -L%p/../libs/MIPS/Android/android-14/ -lc -o %t.mips.so
; RUN: test -f %t.mips.so
