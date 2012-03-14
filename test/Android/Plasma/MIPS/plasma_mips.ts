: RUN: cp %p/../libplasma_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -march=mipsel \
; RUN: -dB libplasma_portable.bc -o libplasma_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=mipsel -soname=libplasma_portable.so \
; RUN: -L=%p/../../../libs/MIPS/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/MIPS/Android/android-14/crtbegin_so.o \
; RUN: libplasma_portable.o \
; RUN: -o libplasma_portable.so \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/MIPS/Android/android-14/crtend_so.o

; RUN: diff -s libplasma_portable.so %p/libplasma_portable.golden.so

