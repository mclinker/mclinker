; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -dB %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -z origin -z nodefaultlib -z nodelete -z nodlopen      \
; RUN: -z nodump -z loadfltr -z interpose -z initfirst        \
; RUN: -filetype=dso %t.o -o %t.so
; RUN: readelf -a %t.so | grep FLAGS | grep ORIGIN
; RUN: readelf -a %t.so | grep FLAGS_1 | grep NODELETE
; RUN: readelf -a %t.so | grep FLAGS_1 | grep LOADFLTR
; RUN: readelf -a %t.so | grep FLAGS_1 | grep INITFIRST
; RUN: readelf -a %t.so | grep FLAGS_1 | grep NOOPEN
; RUN: readelf -a %t.so | grep FLAGS_1 | grep ORIGIN
; RUN: readelf -a %t.so | grep FLAGS_1 | grep INTERPOSE
; RUN: readelf -a %t.so | grep FLAGS_1 | grep NODEFLIB
; RUN: readelf -a %t.so | grep FLAGS_1 | grep NODUMP


@global_i = common global i32 0, align 4

define void @foo() nounwind {
entry:
  store i32 1, i32* @global_i, align 4
  ret void
}
