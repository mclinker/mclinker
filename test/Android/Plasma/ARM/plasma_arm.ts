: RUN: cp %p/../plasma/libplasma_portable.bc ./

; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB libplasma_portable.bc -o Output/libplasma_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libplasma_portable.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o \
; RUN: Output/libplasma_portable.o \
; RUN: -o Output/libplasma_portable.so \
; RUN: -lm -llog -ljnigraphics -lc

; RUN: diff -s Output/libplasma_portable.so %p/../plasma/golden/ARM/libplasma_portable.so

