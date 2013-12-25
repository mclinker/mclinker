; Build the shared library.
; RUN: %MCLinker --shared -soname=libquake.so \
; RUN: -march=arm -mtriple="armv7-none-linux-gnueabi" \
; RUN: -o libquake.llvm.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/quake.o \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm -lstdc++ \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libquake.llvm.so %p/libquake.golden.so
