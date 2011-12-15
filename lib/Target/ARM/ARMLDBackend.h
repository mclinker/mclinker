//===- ARMLDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_LDBACKEND_H
#define ARM_LDBACKEND_H
#include "mcld/Target/GNULDBackend.h"
#include "mcld/Target/GOT.h"
#include "mcld/Target/PLT.h"
#include "ARMRelocationFactory.h"

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
  typedef uint32_t GOTEntryTy;

public:

 ARMGOTEntry() : GOTEntry(sizeof(GOTEntryTy), 0) {
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
/// ARMGNULDBackend - linker backend of ARM target of GNU ELF format
///
class ARMGNULDBackend : public GNULDBackend
{
public:
  ARMGNULDBackend();
  ~ARMGNULDBackend();

  /// getRelocFactory - co-variant return type
  ARMRelocationFactory* getRelocFactory();

  uint32_t machine() const;

private:
  ARMRelocationFactory* m_pRelocFactory;
};

//===----------------------------------------------------------------------===//
/// ARMMachOLDBackend - linker backend of ARM target of MachO format
///
/**
class ARMMachOLDBackend : public DarwinARMLDBackend
{
public:
  ARMMachOLDBackend();
  ~ARMMachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

