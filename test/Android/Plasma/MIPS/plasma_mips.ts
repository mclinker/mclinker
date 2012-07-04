; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=mipsel -soname=libplasma.so \
; RUN: -mtriple="mipsel-none-linux-gnu" \
; RUN: -L=%p/../../../libs/MIPS/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/MIPS/Android/android-14/crtbegin_so.o \
; RUN: %p/plasma.o \
; RUN: -o libplasma.so \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/MIPS/Android/android-14/crtend_so.o

; RUN: diff -s libplasma.so %p/libplasma.golden.so

