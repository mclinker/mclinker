

#include "llvm/mcld/MCELFLDTargetWriter.h"

using namespace mcld;


MCELFLDTargetWriter::MCELFLDTargetWriter(bool Is64Bit_,
                                         Triple::OSType OSType_,
                                         uint16_t EMachine_,
                                         bool HasRelocationAddend_)
  : OSType(OSType_), EMachine(EMachine_),
    HasRelocationAddend(HasRelocationAddend_), Is64Bit(Is64Bit) {
}

MCELFObjectTargetWriter::~MCELFLDTargetWriter() {
}
