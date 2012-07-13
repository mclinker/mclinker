; Build the shared library.
; RUN: clang++ -emit-llvm -c -ccc-host-triple arm-none-linux-gnueabi \
; RUN: %p/gotplt.cpp -o Output/gotplt.bc

; RUN: %MCLinker -filetype=obj \
; RUN: -march=arm -mtriple arm-none-linux-gnueabi \
; RUN: -relocation-model=pic \
; RUN: -dB Output/gotplt.bc -o Output/gotplt.o

; RUN: %MCLinker --shared -soname=libgotplt.so \
; RUN: -march=arm -mtriple arm-none-linux-gnueabi \
; RUN: Output/gotplt.o -o Output/libgotplt.so

; Read data from the shared library for comparison.
; RUN: readelf -a Output/libgotplt.so | \
; RUN: grep -o "\.dynamic *DYNAMIC *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t

; RUN: llvm-objdump -s Output/libgotplt.so | \
; RUN: grep -A 1 "section .got" | \
; RUN: awk 'NR==2{print $2}' | rev | dd conv=swab >> %t

; RUN: cat %t | \
; RUN: FileCheck %s -check-prefix=GOT0

; GOT0: [[ADDR:([0-9a-f]*)]]
; GOT0-NEXT: [[ADDR]]

