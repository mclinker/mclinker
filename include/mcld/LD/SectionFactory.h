//===- header.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SECTIONFACTORY_H
#define SECTIONFACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/MC/MCAssembler.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>

namespace mcld
{

/** \class SectionFactory
 *  \brief provide the interface to create and delete section data for output
 */
class SectionFactory : public GCFactory<llvm::MCSectionData, 0>
{
public:
  SectionFactory(size_t pNum);
  ~SectionFactory();

  llvm::MCSectionData* produce(LDFileFormat::Kind pKind,
                               const std::string& pName,
                               uint32_t pFlag = 0,
                               uint32_t pType = 0);

  void destroy(llvm::MCSectionData* pSD);

private:
  typedef GCFactory<LDSection, 0> SectHeaderFactory;

private:
  SectHeaderFactory m_HeaderFactory;
};

} // namespace of mcld

#endif

