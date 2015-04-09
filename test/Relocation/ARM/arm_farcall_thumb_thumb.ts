; RUN: %MCLinker -mtriple=arm-none-linux-gnueabi -march=arm \
; RUN: %p/arm_farcall_thumb_thumb.o -o %t --section-start=.foo=0x409000
; RUN: readelf -s %t | FileCheck %s
; CHECK: __bar_T2T_veneer@island-0
