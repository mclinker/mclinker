: RUN: cp %p/../libplasma_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic -march=x86 \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -dB libplasma_portable.bc -o libplasma_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=x86 -soname=libplasma_portable.so \
; RUN: -mtriple="x86-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/X86/Android/android-14  -Bsymbolic \
; RUN: libplasma_portable.o \
; RUN: -o libplasma_portable.so \
; RUN: -lm -llog -ljnigraphics -lc

; RUN: diff -s libplasma_portable.so %p/libplasma_portable.golden.so

