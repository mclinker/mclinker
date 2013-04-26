; RUN: %MCLinker -static -mtriple=arm-linux-gnueabi --build-id -o %t.arm.exe
; RUN: test -f %t.arm.exe
