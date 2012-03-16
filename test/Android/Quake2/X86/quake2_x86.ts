: RUN: cp %p/../libquake2_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -dB libquake2_portable.bc -o libquake2_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake2_portable.so \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: libquake2_portable.o \
; RUN: -o libquake2_portable.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s libquake2_portable.so %p/libquake2_portable.golden.so

