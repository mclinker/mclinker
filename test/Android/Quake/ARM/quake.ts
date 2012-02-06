; Build the object file.
; RUN: %MCLinker -filetype=obj -relocation-model=pic \
; RUN: -mtriple="armv7-none-linux-gnueabi" \
; RUN: -dB %p/../libquake_portable.bc -o Output/libquake_portable.o

; RUN: strip -N %p/../libquake_portable.bc Output/libquake_portable.o

; Build the shared library.
; RUN: %MCLinker -filetype=dso -march=arm -soname=libquake_portable.so \
; RUN: -L=%p/../../../libs/ARM/Android/android-14  -Bsymbolic \
; RUN: %p/../../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN: %p/../../../libs/ARM/Android/android-14/crtend_so.o \
; RUN: Output/libquake_portable.o \
; RUN: -dB %p/../libquake_portable.bc -o Output/libquake_portable.so

; RUN: diff -s Output/libquake_portable.so %p/golden/libquake_portable.so | \
; RUN: awk '{print $6}' | \
; RUN: FileCheck %s -check-prefix=Quake

; Quake: identical

