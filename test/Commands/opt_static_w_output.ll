; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -static %t.o -o %t.exe
; RUN: test -f %t.exe

; RUN: readelf -Ss %t.exe | FileCheck %s
; CHECK: [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
; CHECK: [ 0]                   NULL            00000000 000000 000000 00      0   0  0
; CHECK: [ 1] .text             PROGBITS        00008094 000094 000018 00  AX  0   0  4
; CHECK: [ 2] .shstrtab         STRTAB          00000000 0000ac 000041 00      0   0  1
; CHECK: [ 3] .symtab           SYMTAB          00000000 0000f0 0000a0 10      6   6  4
; CHECK: [ 4] .note.GNU-stack   PROGBITS        00000000 000190 000000 00      0   0  1
; CHECK: [ 5] .ARM.attributes   ARM_ATTRIBUTES  00000000 000190 000022 00      0   0  1
; CHECK: [ 6] .strtab           STRTAB          00000000 0001b2 00006b 00      0   0  1

; CHECK:  Num:    Value  Size Type    Bind   Vis      Ndx Name
; CHECK: 0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
; CHECK-NOT: UND

define i32 @main() nounwind {
  %1 = alloca i32, align 4
  store i32 0, i32* %1
  ret i32 0
}

