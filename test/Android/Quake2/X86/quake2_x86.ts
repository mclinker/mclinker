: RUN: cp %p/../quake2/libquake2.bc ../

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -dB ../libquake2.bc -o Output/libquake2.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake2.so \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: Output/libquake2.o \
; RUN: -o Output/libquake2.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s Output/libquake2.so %p/../quake2/golden/X86/libquake2.so | \
; RUN: awk '{print $6}' | \
; RUN: FileCheck %s -check-prefix=Quake

; Quake: identical

