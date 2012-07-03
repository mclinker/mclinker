; Build the shared library.
; RUN: %MCLinker -filetype=dso \
; RUN: -march=arm -soname=libplasma.so \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/plasma.o \
; RUN: -o libplasma.so \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libplasma.so %p/libplasma.golden.so

