; RUN: cp %p/true_f.s %t.ll

; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %t.ll -o %t.1.o
; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.2.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: %t.1.o %t.2.o --wrap=f -o %t.3.o -pie

; RUN: rm %t.1.o
; RUN: rm %t.2.o
; RUN: rm %t.3.o
; RUN: rm %t.ll
target triple = "arm-none-linux-gnueabi"

define i8* @__wrap_f(i32 %c) uwtable ssp {
  %1 = alloca i32, align 4
  store i32 %c, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = call i8* @__real_f(i32 %2)
  ret i8* %3
}

declare i8* @__real_f(i32)
