; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake2.so \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/quake2.o \
; RUN: -o libquake2.so \
; RUN: -lGLESv1_CM -llog -lc -lm  \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libquake2.so %p/libquake2.golden.so

