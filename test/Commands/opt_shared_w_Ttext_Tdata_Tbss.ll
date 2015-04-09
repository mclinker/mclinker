; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -L=%p/../libs/ARM/Android/android-14                   \
; RUN: %p/../libs/ARM/Android/android-14/crtbegin_so.o        \
; RUN: -Bshareable %t.o -o %t.so                              \
; RUN: -lc --as-needed -lm --no-as-needed                     \
; RUN: %p/../libs/ARM/Android/android-14/crtend_so.o          \
; RUN: -Ttext=0x10000 -Tdata=0x20000 -Tbss=0x30000
; RUN: readelf -l %t.so | grep LOAD | FileCheck %s
; CHECK: 0x00010000 0x00010000
; CHECK: 0x00020000 0x00020000
; CHECK: 0x00030000 0x00030000

define i32 @main() nounwind {
  %1 = alloca i32, align 4
  store i32 0, i32* %1
  ret i32 0
}
