: RUN: cp %p/../libquake2.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -dB libquake2.bc -o libquake2.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake2.so \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: libquake2.o \
; RUN: -o libquake2.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s libquake2.so %p/libquake2.golden.so

