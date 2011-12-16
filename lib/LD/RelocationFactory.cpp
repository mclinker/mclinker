//===- RelocationFactory.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/RelocationFactory.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/TargetLDBackend.h>
#include <llvm/Support/Host.h>
#include <cstring>
#include <cassert>

using namespace mcld;

//==========================
// RelocationFactory
RelocationFactory::RelocationFactory(size_t pNum,
                                     TargetLDBackend& pParent)
  : GCFactory<Relocation, 0>(pNum),
    m_pTargetDataFactory(new TargetDataFactory(pNum)),
    m_pLayout(0),
    m_Parent(pParent) {
}

RelocationFactory::~RelocationFactory()
{
  if (0 != m_pTargetDataFactory)
    delete m_pTargetDataFactory;
}

Relocation* RelocationFactory::produce(RelocationFactory::Type pType,
                                       MCFragmentRef& pFragRef,
                                       Address pAddend)
{
  Relocation* result = allocate();

  // If the host and the target have the same endian, directly use the data that
  // FragRef refers to. Otherwise, use TargetDataFactory to generate temporary
  // data.
  DWord* target_data = NULL;
  target_data = m_pTargetDataFactory->allocate();
  std::memcpy(target_data, pFragRef.deref(), sizeof(DWord));

  // make target_data get right value
  // old target_data:
  // For 32-bit machine
  //  63 .. 32  31 .. 0
  // |AAAAAAAA|BBBBBBBB|
  //
  // shift  right 32 bits
  //
  // |00000000|AAAAAAAA|
  (*target_data) >>= (sizeof(DWord)*8 - m_Parent.bitclass());

  new (result) Relocation(pType,
                          pFragRef,
                          pAddend,
                          target_data,
                          *this);
  return result;
}

void RelocationFactory::destroy(Relocation* pRelocation)
{
   /** GCFactory will recycle the relocation **/
}

void RelocationFactory::setLayout(const Layout& pLayout)
{
  m_pLayout = &pLayout;
}

const Layout& RelocationFactory::getLayout() const
{
  assert(0 != m_pLayout);
  return *m_pLayout;
}

GOT& RelocationFactory::getGOT()
{
  return m_Parent.getGOT();
}

const GOT& RelocationFactory::getGOT() const
{
  return m_Parent.getGOT();
}

