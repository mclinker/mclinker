/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_INPUT_H
#define MCLD_INPUT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDFile.h>

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
    Archive = MCLDFile::Archive,
    Object = MCLDFile::Object,
    DynObj = MCLDFile::DynObj,
    Script = MCLDFile::Script,
    Unknown = MCLDFile::Unknown
  };

private:
  explicit Input(llvm::StringRef pName,
                 const AttributeProxy& pAttr);

  Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const AttributeProxy& pAttr,
        unsigned int pType = Unknown);

public:
  ~Input();

  bool isRecognized() const
  { return (m_Type != Unknown); }

  const Attribute* attribute() const
  { return m_pAttr; }

private:
  Attribute *m_pAttr;
};

} // namespace of mcld
 
#endif

