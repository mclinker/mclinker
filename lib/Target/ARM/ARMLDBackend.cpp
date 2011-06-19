
namespace mcld {
class ARMMachObjectWriter : public MCMachObjectTargetWriter {
public:
  ARMMachObjectWriter(bool Is64Bit, uint32_t CPUType,
                      uint32_t CPUSubtype)
    : MCMachObjectTargetWriter(Is64Bit, CPUType, CPUSubtype,
                               /*UseAggressiveSymbolFolding=*/true) {}
};

class ARMELFObjectWriter : public MCELFObjectTargetWriter {
public:
  ARMELFObjectWriter(Triple::OSType OSType)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSType, ELF::EM_ARM,
                              /*HasRelocationAddend*/ false) {}
};


class ARMLDBackend : public TargetLDBackend {

};
//implement ARMLDbackend




class ELFARMLDBackend : public ARMLDBackend {
  
};
//implement member functions of ELFARMLDBackend


class DarwinARMLDBackend : public ARMLDBackend {


};
//implement member functions of DarwinARMLDBackend


TargetLDBackend  *mcld::createARMLDBackend(const Target &T,
                                           const std::string &TT) {

  Triple TheTriple(TT);

  if (TheTriple().isOSDarwin()) {
    if (TheTriple.getArchName() == "armv6" ||
        TheTriple.getArchName() == "thumbv6")
      return DarwinARMLDBackend(T, object::mach::CSARM_V6);
    return DarwinARMLDBackend(T, object::mach::CSARM_V7);

  }

  if (TheTriple.isOSWindows())
    assert(0 && "Windows not supported on ARM");

  return ELFARMLDBackend(T, Triple(TT).getOS());
};



} //end namespace mcld 
