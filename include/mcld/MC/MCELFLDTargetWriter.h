

#ifndef MCLD_MC_MCELFLDTARGETWRITER_H
#define MCLD_MC_MCELFLDTARGETWRITER_H

#include "mcld/MC/MCLDWriter.h"
#include "llvm/Support/DataTypes.h"


namespace mcld{
class MCELFLDTargetWriter {
  const Triple::OSTyoe OSType;
  const uint16_t EMachine;
  const unsigned HasRelocationAddend : 1;
  const Is64Bit : 1;
protected:
  MCELFLDTargetWriter(bool Is64Bit_, Triple::OSType OSType_,
                      uint16_t EMachine_, bool HasRelocationAddend);


public:
  virtual ~MCELFLDTargetWriter();
  Triple::OSType getOSType() { return OSType; }
  uint16_t getEMachine () { return EMachine; }
  bool hasRelocationAddend() { return HasRelocationAddend; }
  bool is64Bit() { return Is64Bit; }


/// brief construct a new ELF LDWriter instance
///
/// param MOTW - The target specific ELF writer subclass.
/// param OS - The stream to write to.
/// returns The constructed ld writer.

MCLDWriter createELFLDWriter(MCELFLDTargetWriter *MOVT,
                             raw_ostream &OS, bool IsLittleEndian); 

} //end namespce mcld



