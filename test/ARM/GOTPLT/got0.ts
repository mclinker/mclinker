; Build the shared library.
; RUN: clang++ -emit-llvm -c -ccc-host-triple arm-none-linux-gnueabi \
; RUN: %p/gotplt.cpp -o Output/gotplt.bc

; RUN: %MCLinker -filetype=obj -march=arm -relocation-model=pic \
; RUN: -dB Output/gotplt.bc -o Output/gotplt.o

; RUN: %MCLinker -filetype=dso -march=arm -soname=libgotplt.so \
; RUN: -dB Output/gotplt.bc Output/gotplt.o -o Output/libgotplt.so

; Read data from the shared library for comparison.
; RUN: readelf -a Output/libgotplt.so | \
; RUN: grep -o "\.dynamic *DYNAMIC *[0-9a-f]*" | \
; RUN: awk '{print $3}' > %t

; RUN: objdump -D Output/libgotplt.so | \
; RUN: grep -A 1 "[0-9a-f]* <_GLOBAL_OFFSET_TABLE_>:" | \
; RUN: awk 'NR==2{print $2}' >> %t

; RUN: cat %t | \
; RUN: FileCheck %s -check-prefix=GOT0

; GOT0: [[ADDR:([0-9a-f]*)]]
; GOT0-NEXT: [[ADDR]]



