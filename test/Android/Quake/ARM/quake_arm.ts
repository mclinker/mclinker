: RUN: cp %p/../libquake_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB libquake_portable.bc -o libquake_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake_portable.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o \
; RUN: libquake_portable.o \
; RUN: -o libquake_portable.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s libquake_portable.so %p/libquake_portable.golden.so

