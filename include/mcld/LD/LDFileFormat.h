//===- LDFileFormat.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDFILE_FORMAT_H
#define MCLD_LDFILE_FORMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class MCLinker;
class LDSection;

/** \class LDFileFormat
 *  \brief LDFileFormat describes the common file formats.
 */
class LDFileFormat
{
public:
  enum Kind {
    Null,
    Regular,
    BSS,
    NamePool,
    Relocation,
    Debug,
    Target,
    Note,
    MetaData,
  };

protected:
  LDFileFormat();

public:
  virtual ~LDFileFormat();

  /// initStdSections - initialize all standard sections.
  void initStdSections(MCLinker& pLinker);

  /// initObjectFormat - different format, such as ELF and MachO, should
  /// implement this
  virtual void initObjectFormat(MCLinker& pLinker) = 0;

  /// initObjectType - different types, such as shared object, executable
  /// files, should implement this
  virtual void initObjectType(MCLinker& pLinker) = 0;

protected:
  //         variable name         :  ELF               MachO
  LDSection* f_pTextSection;       // .text             __text
  LDSection* f_pDataSection;       // .data             __data
  LDSection* f_pBSSSection;        // .bss              __bss
  LDSection* f_pReadOnlySection;   // .rodata           __const
};

} // namespace of mcld

#endif

