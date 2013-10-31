; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: not %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: %t.o -o ./undef_no_output.exe 2>&1 | grep "undefined reference to" | grep "ctor"
; RUN: ! test -f ./undef_no_output.exe

@ctor = external global i32

define i32 @main(i32 %argc, i8** %argv) nounwind uwtable ssp {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  store i32 0, i32* %retval
  store i32 %argc, i32* %argc.addr, align 4
  store i8** %argv, i8*** %argv.addr, align 8
  store i32 10, i32* @ctor, align 4
  %0 = load i32* @ctor, align 4
  ret i32 %0
}
