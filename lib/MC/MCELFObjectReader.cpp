#include "mcld/MC/MCELFObjectReader.h"
#include <vector>
typedef std::vector<std::pair<unsigned char, const char*> > ByteArrayTy;

namespace mcld {

  bool MCELFObjectReader::getMCInstruction(const MCDisassembler &DisAsm,
                                      const ByteArrayTy &Byte) {
    // Wrap the vector in a MemoryObject.
    VectorMemoryObject memoryObject(Bytes);

    // Disassemble it to strings.
    uint64_t Size;
    uint64_t Index;

    for (Index = 0; Index < Bytes.size(); Index += Size) {
      MCInst Inst;

      if (DisAsm.getInstruction(Inst, Size, memoryObject, Index,
                               /*REMOVE*/ nulls())) {
        //FIXME : Add Inst to LD MCInst list
      } else {
        if (Size == 0)
          Size = 1; // skip illegible bytes
      }
    }
  }

} //end namespace mcld
