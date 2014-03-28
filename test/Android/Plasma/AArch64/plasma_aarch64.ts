; Build the shared library.
; RUN: %MCLinker --shared -soname=libplasma.so -Bsymbolic \
; RUN: -march=aarch64 -mtriple="aarch64-none-linux-gnueabi" \
; RUN: -o libplasma.so \
; RUN: -L=%p/../../../libs/AArch64/Android/ \
; RUN: %p/../../../libs/AArch64/Android/crtbegin_so.o \
; RUN: %p/plasma.o \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/AArch64/Android/crtend_so.o

; RUN: diff -s libplasma.so %p/libplasma.golden.so
