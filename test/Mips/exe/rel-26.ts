; RUN: %MCLinker -Bstatic -mtriple=mipsel-linux-gnueabi -filetype=exe \
; RUN:           %p/rel26-32.o -o %t.32.exe
; RUN: llvm-objdump -disassemble %t.32.exe | FileCheck %s -check-prefix=MIPS32

; RUN: %MCLinker -Bstatic -mtriple=mips64el-linux-gnueabi -filetype=exe \
; RUN:           %p/rel26-64.o -o %t.64.exe
; RUN: llvm-objdump -disassemble %t.64.exe | FileCheck %s -check-prefix=MIPS64

; MIPS32:      main:
; MIPS32-NEXT: 400094:  f0 ff bd 27  addiu   $sp, $sp, -16
; MIPS32-NEXT: 400098:  0c 00 be af  sw      $fp, 12($sp)
; MIPS32-NEXT: 40009c:  21 f0 a0 03  move     $fp, $sp
; MIPS32-NEXT: 4000a0:  08 00 c0 af  sw      $zero, 8($fp)
; MIPS32-NEXT: 4000a4:  03 00 01 24  addiu   $1, $zero, 3
; MIPS32-NEXT: 4000a8:  04 00 c1 af  sw      $1, 4($fp)
; MIPS32-NEXT: 4000ac:  04 00 c2 8f  lw      $2, 4($fp)
; MIPS32-NEXT: 4000b0:  07 00 41 14  bne     $2, $1, 32
; MIPS32-NEXT: 4000b4:  00 00 00 00  nop
; MIPS32-NEXT: 4000b8:  30 00 10 08  j       4194496
; MIPS32-NEXT: 4000bc:  00 00 00 00  nop
; MIPS32-NEXT: 4000c0:  0a 00 01 24  addiu   $1, $zero, 10
; MIPS32-NEXT: 4000c4:  08 00 c1 af  sw      $1, 8($fp)
; MIPS32-NEXT: 4000c8:  38 00 10 08  j       4194528
; MIPS32-NEXT: 4000cc:  00 00 00 00  nop
; MIPS32-NEXT: 4000d0:  0b 00 01 24  addiu   $1, $zero, 11
; MIPS32-NEXT: 4000d4:  08 00 c1 af  sw      $1, 8($fp)
; MIPS32-NEXT: 4000d8:  38 00 10 08  j       4194528
; MIPS32-NEXT: 4000dc:  00 00 00 00  nop
; MIPS32-NEXT: 4000e0:  08 00 c2 8f  lw      $2, 8($fp)
; MIPS32-NEXT: 4000e4:  21 e8 c0 03  move     $sp, $fp
; MIPS32-NEXT: 4000e8:  0c 00 be 8f  lw      $fp, 12($sp)
; MIPS32-NEXT: 4000ec:  10 00 bd 27  addiu   $sp, $sp, 16
; MIPS32-NEXT: 4000f0:  08 00 e0 03  jr      $ra
; MIPS32-NEXT: 4000f4:  00 00 00 00  nop

; MIPS64:      main:
; MIPS64-NEXT: 1200000e8:  f0 ff bd 67  daddiu  $sp, $sp, -16
; MIPS64-NEXT: 1200000ec:  08 00 be ff  sd      $fp, 8($sp)
; MIPS64-NEXT: 1200000f0:  2d f0 a0 03  move     $fp, $sp
; MIPS64-NEXT: 1200000f4:  04 00 c0 af  sw      $zero, 4($fp)
; MIPS64-NEXT: 1200000f8:  03 00 01 24  addiu   $1, $zero, 3
; MIPS64-NEXT: 1200000fc:  00 00 c1 af  sw      $1, 0($fp)
; MIPS64-NEXT: 120000100:  00 00 c2 8f  lw      $2, 0($fp)
; MIPS64-NEXT: 120000104:  07 00 41 14  bne     $2, $1, 32
; MIPS64-NEXT: 120000108:  00 00 00 00  nop
; MIPS64-NEXT: 12000010c:  45 00 00 08  j       276
; MIPS64-NEXT: 120000110:  00 00 00 00  nop
; MIPS64-NEXT: 120000114:  0a 00 01 24  addiu   $1, $zero, 10
; MIPS64-NEXT: 120000118:  04 00 c1 af  sw      $1, 4($fp)
; MIPS64-NEXT: 12000011c:  4d 00 00 08  j       308
; MIPS64-NEXT: 120000120:  00 00 00 00  nop
; MIPS64-NEXT: 120000124:  0b 00 01 24  addiu   $1, $zero, 11
; MIPS64-NEXT: 120000128:  04 00 c1 af  sw      $1, 4($fp)
; MIPS64-NEXT: 12000012c:  4d 00 00 08  j       308
; MIPS64-NEXT: 120000130:  00 00 00 00  nop
; MIPS64-NEXT: 120000134:  04 00 c2 8f  lw      $2, 4($fp)
; MIPS64-NEXT: 120000138:  2d e8 c0 03  move     $sp, $fp
; MIPS64-NEXT: 12000013c:  08 00 be df  ld      $fp, 8($sp)
; MIPS64-NEXT: 120000140:  10 00 bd 67  daddiu  $sp, $sp, 16
; MIPS64-NEXT: 120000144:  08 00 e0 03  jr      $ra
; MIPS64-NEXT: 120000148:  00 00 00 00  nop
