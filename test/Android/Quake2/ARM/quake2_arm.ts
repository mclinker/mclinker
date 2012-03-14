: RUN: cp %p/../libquake2_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB libquake2_portable.bc -o libquake2_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake2_portable.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: libquake2_portable.o \
; RUN: -o libquake2_portable.so \
; RUN: -lGLESv1_CM -llog -lc -lm  \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libquake2_portable.so %p/libquake2_portable.golden.so

