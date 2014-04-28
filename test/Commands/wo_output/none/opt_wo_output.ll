; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: %t.o -pie
; RUN: test -f ./a.out
; RUN: readelf -a ./a.out | grep Type | grep DYN | grep "Shared object file"

define i32 @main(i32 %argc, i8** %argv) nounwind uwtable ssp {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  store i32 0, i32* %retval
  store i32 %argc, i32* %argc.addr, align 4
  store i8** %argv, i8*** %argv.addr, align 8
  ret i32 0
}

