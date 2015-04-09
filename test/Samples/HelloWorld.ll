;;; ARM
; .o
; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: llvm-nm %t.o | FileCheck %s -check-prefix=ARM_O_NM
; ARM_O_NM: {{[0-9a-fA-F]+}} T main
; ARM_O_NM:          U puts
; RUN: llvm-objdump -section-headers %t.o | awk '{print $2}' | sort | FileCheck %s -check-prefix=ARM_O_OBJDUMP
; ARM_O_OBJDUMP: .ARM.attributes
; ARM_O_OBJDUMP: .rel.text
; ARM_O_OBJDUMP: .strtab
; ARM_O_OBJDUMP: .symtab
; ARM_O_OBJDUMP: .text
; .so
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -shared %t.o -o %t.so
; RUN: llvm-nm %t.so | FileCheck %s -check-prefix=ARM_SO_NM
; ARM_SO_NM: {{[0-9a-fA-F]+}} T main
; ARM_SO_NM:          U puts
; RUN: llvm-objdump -section-headers %t.so | awk '{print $2}' | sort | FileCheck %s -check-prefix=ARM_SO_OBJDUMP
; ARM_SO_OBJDUMP: .ARM.attributes
; ARM_SO_OBJDUMP: .dynamic
; ARM_SO_OBJDUMP: .dynstr
; ARM_SO_OBJDUMP: .dynsym
; ARM_SO_OBJDUMP: .got
; ARM_SO_OBJDUMP: .plt
; ARM_SO_OBJDUMP: .rel.plt
; ARM_SO_OBJDUMP: .text

; Hello world from LLVM Language Reference Manual.

; Declare the string constant as a global constant.
@.LC0 = internal constant [13 x i8] c"hello world\0A\00"      ; [13 x i8]*

; External declaration of the puts function
declare i32 @puts(i8*)                                      ; i32 (i8*)*

; Definition of main function
define i32 @main() {   ; i32()*
  ; Convert [13 x i8]* to i8  *...
  %cast210 = getelementptr [13 x i8], [13 x i8]* @.LC0, i64 0, i64 0   ; i8*

  ; Call puts function to write out the string to stdout.
  call i32 @puts(i8* %cast210)           ; i32
  ret i32 0
}
