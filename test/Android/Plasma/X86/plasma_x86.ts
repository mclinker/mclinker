; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libplasma.so \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: %p/plasma.o \
; RUN: -o libplasma.so \
; RUN: -lm -llog -ljnigraphics -lc

; RUN: diff -s libplasma.so %p/libplasma.golden.so
