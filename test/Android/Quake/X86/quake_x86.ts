: RUN: cp %p/../quake/libquake_portable.bc ../

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -dB ../libquake_portable.bc -o Output/libquake_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libquake_portable.so \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: Output/libquake_portable.o \
; RUN: -o Output/libquake_portable.so \
; RUN: -llog -lEGL -lGLESv1_CM -lOpenSLES -lc -lm

; RUN: diff -s Output/libquake_portable.so %p/../quake/golden/X86/libquake_portable.so | \
; RUN: awk '{print $6}' | \
; RUN: FileCheck %s -check-prefix=Quake

; Quake: identical

