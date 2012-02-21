: RUN: cp %p/../libquake_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -dB libquake_portable.bc -o libquake_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake_portable.so \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: libquake_portable.o \
; RUN: -o libquake_portable.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s libquake_portable.so %p/libquake_portable.golden.so

