; RUN: cp %p/true_f.s ./true_f.ll

; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -fPIC -dB ./true_f.ll -o %t.1.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -fPIC -dB %s -o %t.2.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: %t.1.o %t.2.o --wrap f
target triple = "arm-none-linux-gnueabi"

define i8* @_Z8__wrap_fj(i32 %c) uwtable ssp {
  %1 = alloca i32, align 4
  store i32 %c, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = call i8* @_Z8__real_fj(i32 %2)
  ret i8* %3
}

declare i8* @_Z8__real_fj(i32)
