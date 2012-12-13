//===- GNUInfo.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_GNU_INFO_H
#define MCLD_TARGET_GNU_INFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/ELF.h>

namespace mcld {

/** \class GNUInfo
 *  \brief GNUInfo records ELF-dependent and target-dependnet data fields
 */
class GNUInfo
{
public:
  /// ELFVersion - the value of e_ident[EI_VERSION]
  virtual uint8_t ELFVersion() const { return llvm::ELF::EV_CURRENT; }

  /// The return value of machine() it the same as e_machine in the ELF header
  virtual uint32_t machine() const = 0;

  /// OSABI - the value of e_ident[EI_OSABI]
  virtual uint8_t OSABI() const { return llvm::ELF::ELFOSABI_NONE; }
};

} // namespace of mcld

#endif

