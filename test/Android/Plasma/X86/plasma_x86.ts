: RUN: cp %p/../plasma/libplasma_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -dB libplasma_portable.bc -o Output/libplasma_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libplasma_portable.so \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: Output/libplasma_portable.o \
; RUN: -o Output/libplasma_portable.so \
; RUN: -lm -llog -ljnigraphics -lc

; RUN: diff -s Output/libplasma_portable.so %p/../plasma/golden/X86/libplasma_portable.so | \
; RUN: awk '{print $6}' | \
; RUN: FileCheck %s -check-prefix=Plasma

; Plasma: identical

