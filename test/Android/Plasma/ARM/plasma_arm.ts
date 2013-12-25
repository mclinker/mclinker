; Build the shared library.
; RUN: %MCLinker --shared -soname=libplasma.so -Bsymbolic \
; RUN: -march=arm -mtriple="armv7-none-linux-gnueabi" \
; RUN: -o libplasma.llvm.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14 \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/plasma.o \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libplasma.llvm.so %p/libplasma.golden.so
