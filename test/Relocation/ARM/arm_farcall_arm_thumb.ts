; RUN: %MCLinker -mtriple=arm-none-linux-gnueabi -march=arm \
; RUN: %p/arm_farcall_arm_thumb.o -o %t --section-start=.foo=0x2009000
; RUN: readelf -s %t | FileCheck %s
; CHECK: __bar_A2T_veneer@island-0
