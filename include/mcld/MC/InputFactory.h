//===- InputFactory.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_INPUT_FACTORY_H
#define MCLD_INPUT_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/AttributeSet.h>
#include <mcld/MC/Attribute.h>

namespace mcld {

/** \class InputFactory
 *  \brief InputFactory controls the production and destruction of
 *  MCLDInput.
 *
 *  All MCLDFiles created by MCLDFileFactory are guaranteed to be destructed
 *  while MCLDFileFactory is destructed.
 *
 *  FIXME: the number of the Inputs should be passed in by Target or any
 *  target specific class.
 *
 *  \see llvm::sys::Path
 */
class InputFactory : public GCFactory<Input,0>
{
public:
  typedef GCFactory<Input, 0> Alloc;

public:
  explicit InputFactory(size_t pNum);
  ~InputFactory();

  // -----  production  ----- //
  Input* produce(llvm::StringRef pName,
                 const sys::fs::Path& pPath,
                 unsigned int pType = Input::Unknown,
                 off_t pFileOffset = 0);

  // predefined - return the predefined attribute
  const Attribute& predefined() const { return m_Predefined; }
  Attribute&       predefined()       { return m_Predefined; }

  // constraint - return the constraint of attributes
  const AttrConstraint& constraint() const { return m_Constraint; }
  AttrConstraint&       constraint()       { return m_Constraint; }

  // last - the last touched attribute.
  const AttributeProxy& last() const { return *m_pLast; }
  AttributeProxy&       last()       { return *m_pLast; }

  bool checkAttributes() const;

private:
  Attribute m_Predefined;
  AttrConstraint m_Constraint;
  AttributeProxy* m_pLast;

  AttributeFactory* m_pAttrFactory;
};

} // namespace of mcld

#endif

