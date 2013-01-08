; XFAIL:*
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -L=%p/../libs/ARM/Android/android-14                   \
; RUN: %p/../libs/ARM/Android/android-14/crtbegin_so.o        \
; RUN: -Bshareable %t.o -o %t.so                              \
; RUN: -lc --as-needed -lm --no-as-needed                     \
; RUN: %p/../libs/ARM/Android/android-14/crtend_so.o
; RUN: readelf -d %t.so | grep NEEDED | FileCheck %s
; CHECK: libc
; CHECK-NOT: libm

define i32 @main() nounwind {
  %1 = alloca i32, align 4
  store i32 0, i32* %1
  ret i32 0
}
