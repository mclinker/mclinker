//===- ARMGOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_GOT_H
#define MCLD_ARM_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/GOT.h>

namespace mcld
{

class ARMGOTEntry : public GOTEntry {
  typedef uint32_t GOTEntryType;

public:

 ARMGOTEntry() : GOTEntry(sizeof(GOTEntryType), 0) {
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

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 */
class ARMGOT : public GOT
{
};

} // namespace of mcld

#endif

