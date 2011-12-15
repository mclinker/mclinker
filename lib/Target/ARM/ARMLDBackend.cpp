//===- ARMLDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARM.h"
#include "ARMLDBackend.h"
#include "mcld/Target/GOT.h"
#include "mcld/Target/PLT.h"
#include "mcld/Support/TargetRegistry.h"
#include "llvm/ADT/Triple.h"

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
  : m_pRelocFactory(0) {
}

ARMGNULDBackend::~ARMGNULDBackend()
{
}

ARMRelocationFactory* ARMGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new ARMRelocationFactory(1024);
  return m_pRelocFactory;
}

namespace mcld {

static const uint32_t arm_plt0[] = {
  0xe52de004, // str   lr, [sp, #-4]!
  0xe59fe004, // ldr   lr, [pc, #4]
  0xe08fe00e, // add   lr, pc, lr
  0xe5bef008, // ldr   pc, [lr, #8]!
  0x00000000, // &GOT[0] - .
};

static const uint32_t arm_plt1[] = {
  0xe28fc600, // add   ip, pc, #0xNN00000
  0xe28cca00, // add   ip, ip, #0xNN000
  0xe5bcf000, // ldr   pc, [ip, #0xNNN]!
};

class ARMGOTEntry : public GOTEntry {
public:
 ARMGOTEntry() : GOTEntry(sizeof(uint32_t), 0) {
   initGOTEntry();
 }

 ~ARMGOTEntry() {
   delete m_pContent;
   m_pContent = 0;
 }

 void initGOTEntry() {
   m_pContent = static_cast<unsigned char*>(malloc(m_EntrySize));
 }
};

class ARMPLT0 : public PLTEntry {
public:
  ARMPLT0(): PLTEntry(sizeof(arm_plt0), 0) {
    initPLTEntry();
  }

  ~ARMPLT0() {
    delete m_pContent;
    m_pContent = 0;
  }

  void initPLTEntry() {
    m_pContent = static_cast<unsigned char*>(malloc(m_EntrySize));

    if (!m_pContent)
      memcpy(m_pContent, arm_plt0, sizeof(arm_plt0));
  }
};

class ARMPLT1 : public PLTEntry {
public:
  ARMPLT1() : PLTEntry(sizeof(arm_plt1),0) {
    initPLTEntry();
  };

  ~ARMPLT1() {
    delete m_pContent;
    m_pContent = 0;
  }

  void initPLTEntry() {
    m_pContent = static_cast<unsigned char*>(malloc(m_EntrySize));

    if (!m_pContent)
      memcpy(m_pContent, arm_plt1, sizeof(arm_plt1));
  }
};

//===----------------------------------------------------------------------===//
/// createARMLDBackend - the help funtion to create corresponding ARMLDBackend
///
TargetLDBackend* createARMLDBackend(const llvm::Target& pTarget,
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new ARMMachOLDBackend(createARMMachOArchiveReader,
                               createARMMachOObjectReader,
                               createARMMachOObjectWriter);
    **/
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new ARMCOFFLDBackend(createARMCOFFArchiveReader,
                               createARMCOFFObjectReader,
                               createARMCOFFObjectWriter);
    **/
  }
  return new ARMGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeARMLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheARMTarget, createARMLDBackend);
}
