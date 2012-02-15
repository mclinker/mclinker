: RUN: cp %p/../quake2/libquake2.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB libquake2.bc -o Output/libquake2.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake2.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o \
; RUN: Output/libquake2.o \
; RUN: -o Output/libquake2.so \
; RUN: -lGLESv1_CM -llog -lc -lm

; RUN: diff -s Output/libquake2.so %p/../quake2/golden/ARM/libquake2.so

