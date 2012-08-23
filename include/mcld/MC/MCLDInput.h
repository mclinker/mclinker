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

#include <mcld/MC/MCLDFile.h>
#include <mcld/Support/Path.h>

namespace mcld {

class MemoryArea;
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
    Unknown,
    Object,
    Exec,
    DynObj,
    CoreFile,
    Script,
    Archive,
    External
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

  const std::string& name() const
  { return m_Name; }

  void setName(const std::string& pName)
  { m_Name = pName; }

  const sys::fs::Path& path() const
  { return m_Path; }

  void setPath(const sys::fs::Path& pPath)
  { m_Path = pPath; }

  void setType(unsigned int pType)
  { m_Type = pType; }

  unsigned int type() const
  { return m_Type; }

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

  // -----  memory area  ----- //

  void setMemArea(MemoryArea* pMemArea)
  { m_pMemArea = pMemArea; }

  bool hasMemArea() const
  { return (NULL != m_pMemArea); }

  const MemoryArea* memArea() const { return m_pMemArea; }
  MemoryArea*       memArea()       { return m_pMemArea; }

private:
  unsigned int m_Type;
  std::string m_Name;
  sys::fs::Path m_Path;
  Attribute *m_pAttr;
  bool m_bNeeded;
  off_t m_fileOffset;
  MemoryArea* m_pMemArea;
};

} // namespace of mcld

#endif

