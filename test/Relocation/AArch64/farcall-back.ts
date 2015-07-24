; RUN: %MCLinker -mtriple=aarch64-linux-android %p/farcall-back.o \
; RUN: -o %t -Ttext=0x1000 --section-start=.foo=0x100000000
; RUN: readelf -sW %t | FileCheck %s
; CHECK: __bar1_ljmp_veneer@island-0
; CHECK: __bar2_ljmp_veneer@island-0
; CHECK: __bar3_ljmp_veneer@island-0
; CHECK: ___start_ljmp_veneer@island-1
; CHECK: ___back_ljmp_veneer@island-1
