; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake.so \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/quake.o \
; RUN: -o libquake.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm -lstdc++ \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libquake.so %p/libquake.golden.so

