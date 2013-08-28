; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -Bshareable %t.o -o %t.so -z execstack
; RUN: readelf -Wl %t.so | grep GNU_STACK | grep -q RWE

; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -Bshareable %t.o -o %t.so -z noexecstack
; RUN: readelf -Wl %t.so | grep GNU_STACK | grep -qv RWE

define i32 @main() nounwind {
  %1 = alloca i32, align 4
  store i32 0, i32* %1
  ret i32 0
}
