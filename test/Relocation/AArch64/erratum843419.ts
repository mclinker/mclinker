; RUN: %MCLinker -mtriple=aarch64-linux-android %p/erratum843419.o \
; RUN: -o %t -e 0 -Ttext=0x400000 -Tdata=0x40000000 \
; RUN: --section-start=.e843419=0x20000000 --section-start=.e835769=0x3000000 \
; RUN: --fix-cortex-a53-835769 --fix-cortex-a53-843419
; RUN: readelf -sW %t | FileCheck %s
; CHECK: __erratum_843419_veneer0@island-1
; CHECK: __erratum_835769_veneer0@island-2
; CHECK: __e843419_adrp_veneer@island-0
; CHECK: __e835769_adrp_veneer@island-1
