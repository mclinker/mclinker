; RUN: %MCLinker -march=arm -shared %p/capget.o %p/capset.o -o %t.out
; RUN: readelf -a %t.out > %t.readelf
; RUN: readelf -w %t.out > %t.dwarf

; Check size
; RUN: size --format=SysV %t.out | grep eh_frame | wc -l > %t.size
; RUN: size --format=SysV -x %t.out | grep eh_frame | awk '{print $2}' >> %t.size
; RUN: cat %t.size | FileCheck %s -check-prefix=SIZE
; SIZE: 1
; SIZE-NEXT: 0x3c

; Check offsets of CIE/FDE are correct
; RUN: cat %t.dwarf | grep CIE > %t.offset
; RUN: cat %t.dwarf | grep FDE >> %t.offset
; RUN: cat %t.offset | FileCheck %s -check-prefix=OFFSET
; OFFSET: 00000000 00000010 00000000 CIE
; OFFSET-NEXT: 00000014 00000010 00000018 FDE cie=00000000
; OFFSET-NEXT: 00000028 00000010 0000002c FDE cie=00000000

; Check FDE pointing to correct position
; RUN: ADDR=`size --format=SysV -x %t.out | grep text | awk '{print $3}' | sed 's/0x//' | tr 'a-z' 'A-Z'`
; RUN: echo "obase=16; ibase=16; $ADDR" | bc > %t.addr
; RUN: echo "obase=16; ibase=16; $ADDR + 20" | bc >> %t.addr
; RUN: echo "obase=16; ibase=16; $ADDR + 40" | bc >> %t.addr
; RUN: cat %t.dwarf | grep FDE | awk -F '=' '{print $3}' | tr '.' '\n' \
; RUN: | sort -n | tail -n 4 | sed -e 's/00000//' | tr 'a-z' 'A-Z' >> %t.addr
; RUN: cat %t.addr | FileCheck %s -check-prefix=ADDR
; ADDR: [[ADDR_1:([0-9A-Z]+)]]
; ADDR-NEXT: [[ADDR_2:([0-9A-Z]+)]]
; ADDR-NEXT: [[ADDR_3:([0-9A-Z]+)]]
; ADDR-NEXT: [[ADDR_1]]
; ADDR-NEXT: [[ADDR_2]]
; ADDR-NEXT: [[ADDR_2]]
; ADDR-NEXT: [[ADDR_3]]

; Check local symbol pointing to correct FDE
; RUN: cat %t.readelf | grep '\$d' | sed -n '0~3p' | awk '{print $2}' \
; RUN: | sed -e 's/00000//' | tr 'a-z' 'A-Z' > %t.addr
; RUN: ADDR=`size --format=SysV -x %t.out | grep eh_frame | awk '{print $3}' | sed 's/0x//' | tr 'a-z' 'A-Z'`
; RUN: PLUS_1=`cat %t.dwarf | grep FDE | head -n 1 | awk '{print $1}' | sed -e 's/000000//'`
; RUN: PLUS_2=`cat %t.dwarf | grep FDE | tail -n 1 | awk '{print $1}' | sed -e 's/000000//'`
; RUN: echo "obase=16; ibase=16; $ADDR + $PLUS_1" | bc >> %t.addr
; RUN: echo "obase=16; ibase=16; $ADDR + $PLUS_2" | bc >> %t.addr
; RUN: cat %t.addr | FileCheck %s -check-prefix=ADDR_SYMBOL
; ADDR_SYMBOL: [[ADDR_1:([0-9A-Z]+)]]
; ADDR_SYMBOL-NEXT: [[ADDR_2:([0-9A-Z]+)]]
; ADDR_SYMBOL-NEXT: [[ADDR_2]]
; ADDR_SYMBOL-NEXT: [[ADDR_1]]
