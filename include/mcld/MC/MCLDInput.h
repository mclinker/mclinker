//===- MCLDInput.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Input class inherits MCLDFile, which is used to represent a input file
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_INPUT_H
#define MCLD_INPUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCLDFile.h"

namespace mcld
{

class AttributeProxy;
class Attribute;
class InputFactory;

/** \class Input
 *  \brief Input provides the information of a input file.
 *
 *  @see MCLDFile
 */
class Input : public MCLDFile
{
friend class InputFactory;
public:
  enum Type {
    Unknown = MCLDFile::Unknown,
    Object = MCLDFile::Object,
    DynObj = MCLDFile::DynObj,
    Archive = MCLDFile::Archive,
    Script = MCLDFile::Script,
    External = MCLDFile::External
  };

private:
  explicit Input(llvm::StringRef pName,
                 const AttributeProxy& pAttr);

  Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const AttributeProxy& pAttr,
        unsigned int pType = Unknown,
        off_t pFileOffset = 0);

public:
  ~Input();

  bool isRecognized() const
  { return (m_Type != Unknown); }

  const Attribute* attribute() const
  { return m_pAttr; }

  bool isNeeded() const
  { return m_bNeeded; }

  void setNeeded()
  { m_bNeeded = true; }

  off_t fileOffset() const
  { return m_fileOffset; }

  void setFileOffset(off_t pFileOffset)
  { m_fileOffset = pFileOffset; }

private:
  Attribute *m_pAttr;
  bool m_bNeeded;
  off_t m_fileOffset;
};

} // namespace of mcld

#endif

