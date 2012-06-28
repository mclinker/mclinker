//===- MCLDInfo.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDINFO_H
#define MCLD_LDINFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/Triple.h>

#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDOptions.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/AttributeFactory.h>
#include <mcld/MC/ContextFactory.h>
#include <mcld/LD/NamePool.h>

#include <string>
#include <cassert>

namespace mcld
{
class Resolver;

/** \class MCLDInfo
 *  \brief MCLDInfo is composed of argumments of MCLinker.
 *   options()        - the general options
 *   scripts()        - the script options
 *   inputs()         - the tree of inputs
 *   bitcode()        - the bitcode being linked
 *   output()         - the output file
 *   inputFactory()   - the list of all inputs
 *   attrFactory()    - the list of all attributes
 *   contextFactory() - the list of all contexts.
 *   memAreaFactory() - the list of all MemoryAreas.
 */
class MCLDInfo
{
public:
  explicit MCLDInfo(const std::string &pTripleString,
                    size_t pAttrNum,
                    size_t InputSize);

  virtual ~MCLDInfo();

  GeneralOptions& options()
  { return m_Options; }

  const GeneralOptions& options() const
  { return m_Options; }

  ScriptOptions& scripts()
  { return m_Scripts; }

  const ScriptOptions& scripts() const
  { return m_Scripts; }

  void setBitcode(const Input& pInput);
  Input& bitcode();
  const Input& bitcode() const;

  Output& output()
  { return *m_pOutput; }

  const Output& output() const
  { return *m_pOutput; }

  InputTree& inputs()
  { return *m_pInputTree; }

  const InputTree& inputs() const
  { return *m_pInputTree; }

  InputFactory& inputFactory()
  { return *m_pInputFactory; }

  const InputFactory& inputFactory() const
  { return *m_pInputFactory; }

  AttributeFactory& attrFactory()
  { return *m_pAttrFactory; }


  const AttributeFactory& attrFactory() const
  { return *m_pAttrFactory; }

  ContextFactory& contextFactory()
  { return *m_pCntxtFactory; }

  const ContextFactory& contextFactory() const
  { return *m_pCntxtFactory; }

  const llvm::Triple& triple() const
  { return m_Triple; }

  static const char* version();

  NamePool& getNamePool() {
    assert(NULL != m_pNamePool);
    return *m_pNamePool;
  }

  const NamePool& getNamePool() const {
    assert(NULL != m_pNamePool);
    return *m_pNamePool;
  }

private:
  // -----  General Options  ----- //
  GeneralOptions m_Options;
  ScriptOptions m_Scripts;
  InputTree *m_pInputTree;
  Input* m_pBitcode;
  Output* m_pOutput;
  llvm::Triple m_Triple;

  // -----  factories  ----- //
  InputFactory *m_pInputFactory;
  AttributeFactory *m_pAttrFactory;
  ContextFactory *m_pCntxtFactory;

  // -----  string and symbols  ----- //
  Resolver* m_pResolver;
  NamePool* m_pNamePool;
};

} // namespace of mcld

#endif

