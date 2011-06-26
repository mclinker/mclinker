#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCInst.h"

namespace mcld {

class MCELFObjectReader {
public:
  bool getMCInstruction(const MCDisassembler &DisAsm, 
                        const ByteArrayTy &Bytes);

};


} //end namespace mcld
