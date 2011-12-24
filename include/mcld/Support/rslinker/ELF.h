#ifndef RS_ELF_H
#define RS_ELF_H

#include <llvm/Support/ELF.h>

// TODO: These definitions are not defined in include/llvm/Support/ELF.h
// elf.h.  So we to this by ourself.  Maybe we should update ELF.h
// instead.

enum {
  ET_LOOS = 0xfe00,
  ET_HIOS = 0xfeff
};

#endif // RS_ELF_H

