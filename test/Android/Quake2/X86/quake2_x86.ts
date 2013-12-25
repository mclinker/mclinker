; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake2.so \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: %p/quake2.o \
; RUN: -o libquake2.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s libquake2.so %p/libquake2.golden.so
