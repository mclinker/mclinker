; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm -z nocopyreloc\
; RUN: -filetype=exe %t.o %p/copy.so -o %t.exe
; RUN: readelf -r %t.exe | FileCheck %s
; CHECK-NOT: R_ARM_COPY
; CHECK: R_ARM_ABS32 {{[0-9a-fA-F]+}} global_exetern_i
; CHECK-NOT: R_ARM_COPY

@global_exetern_i = external global i32

define i32 @main() nounwind {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval
  %0 = load i32* @global_exetern_i, align 4
  ret i32 %0
}
