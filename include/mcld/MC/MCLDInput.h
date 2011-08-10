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
#include <mcld/ADT/GCFactory.h>

namespace mcld
{

class MCLDAttribute;

/** \class Input
 *  \brief Input provides the information of a input file.
 *
 *  @see MCLDFile
 */
class Input : public MCLDFile
{
public:
  enum Type {
    Archive = MCLDFile::Archive,
    Object = MCLDFile::Object,
    DynObj = MCLDFile::DynObj,
    Script = MCLDFile::Script,
    Unknown = MCLDFile::Unknown
  };

public:
  Input();
  Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const MCLDAttribute& pAttr,
        unsigned int pType = Unknown);

  ~Input();

  bool isRecognized() const
  { return (m_Type != Unknown); }

  const MCLDAttribute* attribute() const
  { return m_pAttr; }

  void setAttribute(const MCLDAttribute& pAttr)
  { m_pAttr = const_cast<MCLDAttribute*>(&pAttr); }

private:
  MCLDAttribute *m_pAttr;
};

/** \class InputFactory
 *  \brief InputFactory controls the production and destruction of
 *  MCLDInput.
 *
 *  All MCLDFiles created by MCLDFileFactory are guaranteed to be destructed
 *  while MCLDFileFactory is destructed.
 *
 *  \see llvm::sys::Path
 */
template<size_t NUM>
class InputFactory : public GCFactory<Input, NUM>
{
public:
  typedef GCFactory<Input, NUM> Alloc;

public:
  // -----  production  ----- //
  Input* produce(llvm::StringRef pName,
                 const sys::fs::Path& pPath,
                 const MCLDAttribute& pAttr,
                 unsigned int pType = Input::Unknown);
};

} // namespace of mcld

//===----------------------------------------------------------------------===//
// Template Functions
template<size_t NUM>
mcld::Input* mcld::InputFactory<NUM>::produce(llvm::StringRef pName,
                                              const sys::fs::Path& pPath,
                                              const MCLDAttribute& pAttr,
                                              unsigned int pType)
{
    mcld::Input* result = Alloc::allocate();
    new (result) mcld::Input(pName, pPath, pAttr, pType);
    return result;
}
 
#endif

