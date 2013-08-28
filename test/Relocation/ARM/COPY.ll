; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=exe %t.o  %p/copy.so -o %t.exe
; RUN: readelf -r %t.exe | FileCheck %s
; CHECK: R_ARM_COPY {{[0-9a-fA-F]+}} global_exetern_i

; RUN: readelf -S %t.exe > %t.txt
; RUN: readelf -s %t.exe >> %t.txt
; RUN: cat %t.txt | awk 'BEGIN {BSS_START = -1; SYM_ADDR = -2;} \
; RUN: {if ($3 == ".bss")                     \
; RUN:    BSS_START = $5;}                    \
; RUN: {if ($8 == "global_exetern_i")         \
; RUN:    SYM_ADDR=$2;}                       \
; RUN: END{if (BSS_START != SYM_ADDR) {       \
; RUN:       exit -1;}}'

@global_exetern_i = external global i32

define i32 @main() nounwind {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval
  %0 = load i32* @global_exetern_i, align 4
  ret i32 %0
}
