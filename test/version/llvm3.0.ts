RUN: %MCLinker -version | FileCheck %s
RUN: llvm-objdump -version | FileCheck %s
CHECK: llvm version 3.0
