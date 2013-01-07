;;; ARM
; .o
; RUN: %FakeLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o

; Hello world from LLVM Language Reference Manual.

; Declare the string constant as a global constant.
@.LC0 = internal constant [13 x i8] c"hello world\0A\00"      ; [13 x i8]*

; External declaration of the puts function
declare i32 @puts(i8*)                                      ; i32 (i8*)*

; Definition of main function
define i32 @main() {   ; i32()*
  ; Convert [13 x i8]* to i8  *...
  %cast210 = getelementptr [13 x i8]* @.LC0, i64 0, i64 0   ; i8*

  ; Call puts function to write out the string to stdout.
  call i32 @puts(i8* %cast210)           ; i32
  ret i32 0
}
