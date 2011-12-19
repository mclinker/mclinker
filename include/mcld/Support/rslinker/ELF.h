#ifndef ELF_H
#define ELF_H

#include <llvm/Support/ELF.h>
// FIXME: XXX: Can't using namespace in header file!
using namespace llvm::ELF;

// TODO: These definitions are not defined in include/llvm/Support/ELF.h
// elf.h.  So we to this by ourself.  Maybe we should update ELF.h
// instead.

#ifndef ET_LOOS
#define ET_LOOS 0xfe00
#endif

#ifndef ET_HIOS
#define ET_HIOS 0xfeff
#endif

#ifndef STT_LOOS
#define STT_LOOS 10
#endif

#ifndef STT_HIOS
#define STT_HIOS 12
#endif

#ifndef STB_LOOS
#define STB_LOOS 10
#endif

#ifndef STB_HIOS
#define STB_HIOS 12
#endif


#endif // ELF_H
