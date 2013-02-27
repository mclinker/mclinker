; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -static %t.o -o %t.exe
; RUN: test -f %t.exe

; RUN: readelf -Ss %t.exe | FileCheck %s
; CHECK: [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
; CHECK: [ 0]                   NULL            00000000 000000 000000 00      0   0  0
; CHECK-NOT: .interp
; CHECK-NOT: .dynsym
; CHECK-NOT: .dynstr
; CHECK-NOT: .hash
; CHECK-NOT: .rel
; CHECK-NOT: .dynamic

; CHECK:  Num:    Value  Size Type    Bind   Vis      Ndx Name
; CHECK: 0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
; CHECK-NOT: UND

define i32 @main() nounwind {
  %1 = alloca i32, align 4
  store i32 0, i32* %1
  ret i32 0
}

