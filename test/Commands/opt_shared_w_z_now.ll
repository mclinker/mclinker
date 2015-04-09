; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm -z now\
; RUN: -shared %t.o -o %t.so
; RUN: readelf -a %t.so | grep FLAGS_1 | FileCheck %s
; CHECK: NOW

@global_i = common global i32 0, align 4

define void @foo() nounwind {
entry:
  store i32 1, i32* @global_i, align 4
  ret void
}

