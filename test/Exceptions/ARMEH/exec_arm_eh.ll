; RUN: %MCLinker --eh-frame-hdr -z relro -mtriple=armv7-none-linux-gnueabi        \
; RUN: -dynamic-linker /system/bin/linker                                         \
; RUN: -march=arm %p/../../libs/ARM/Android/android-14/crtbegin_static.o          \
; RUN: %p/exception_main.o %p/../../libs/ARM/Android/android-14/crtend_android.o  \
; RUN: -L%p/../../libs/ARM/Android/android-14 \
; RUN: -L%p/../../libs/ARM/Android/cxx-stl    \
; RUN: -lstdc++ -lm -lc -lgnustl_shared       \
; RUN: -o %t.out


; check EXIDX segment address
; get .ARM.exidx address
; RUN: readelf -S %t.out | grep -o "\.ARM.exidx *ARM_EXIDX *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t.txt

; get segment address
; RUN: readelf -l %t.out | grep "EXIDX [0-9a-f]*" | \
; RUN: awk '{print $3}' >> %t.txt

; compare these two address, they should be the same
; RUN: cat %t.txt | FileCheck %s -check-prefix=SEG
; SEG: [[ADDR:([0-9a-f]*)]]
; SEG-NEXT: 0x[[ADDR]]


; check .ARM.exidx describes to the correct code fragment
; RUN: readelf -u %t.out | FileCheck %s -check-prefix=CONT
; CONT: _Z1fv
; CONT: main
; CONT: _Z41__static_initialization_and_destruction_0ii
; CONT: _GLOBAL__I_exception_main.cpp

