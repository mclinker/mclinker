; XFAIL: *
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -Bshareable %t.o -o ./Bshareable_out_name.so -z defs

@ctor = external global i32

define i32 @_Z1fv() nounwind uwtable ssp {
entry:
  store i32 10, i32* @ctor, align 4
  %0 = load i32* @ctor, align 4
  ret i32 %0
}
