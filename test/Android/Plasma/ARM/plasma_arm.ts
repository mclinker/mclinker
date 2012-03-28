: RUN: cp %p/../libplasma_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB libplasma_portable.bc -o libplasma_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso \
; RUN: -march=arm -soname=libplasma_portable.so \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: libplasma_portable.o \
; RUN: -o libplasma_portable.so \
; RUN: -lm -llog -ljnigraphics -lc  \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o

; RUN: diff -s libplasma_portable.so %p/libplasma_portable.golden.so

