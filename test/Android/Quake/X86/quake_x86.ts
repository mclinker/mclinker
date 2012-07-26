; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake.so \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: %p/quake.o \
; RUN: -o libquake.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm -lstdc++

; RUN: diff -s libquake.so %p/libquake.golden.so

; RUN: %AloneLinker --shared -soname=libquake.so \
; RUN: -mtriple="i386-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: %p/quake.o \
; RUN: -o libquake.mcld.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm -lstdc++

; RUN: diff -s libquake.mcld.so %p/libquake.golden.so
