; create a temporary script to test INPUT command
; RUN: echo "SEARCH_DIR(%p/../libs/ARM/Android/android-14)         \
; RUN:       INPUT(%p/../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN:             %p/../Android/Plasma/ARM/plasma.o               \
; RUN:             -lm -llog -ljnigraphics -lc                     \
; RUN:             %p/../libs/ARM/Android/android-14/crtend_so.o)" \
; RUN: > %t.x

; Build the plasma shared library.
; RUN: %MCLinker --shared -soname=libplasma.so -Bsymbolic \
; RUN: -march=arm -mtriple="armv7-none-linux-gnueabi" \
; RUN: -o %t.out \
; RUN: %t.x

; RUN: diff -s %t.out %p/../Android/Plasma/ARM/libplasma.golden.so
