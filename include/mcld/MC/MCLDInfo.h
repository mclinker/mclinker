/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   csmon7507 <csmon7507@gmail.com>                                         *
 ****************************************************************************/
#ifndef MCLD_LDINFO_H
#define MCLD_LDINFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <llvm/ADT/Triple.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDOptions.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDInputTree.h>
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>
#include <mcld/MC/ContextFactory.h>

namespace mcld
{

/** \class MCLDInfo
 *  \brief MCLDInfo is composed of argumments of MCLDDriver.
 *   options()        - the general options.
 *   inputs()         - the tree of inputs
 *   bitcode()        - the bitcode being linked
 *   output()         - the output file
 *   inputFactory()   - the list of all inputs
 *   attrFactory()    - the list of all attributes
 *   contextFactory() - the list of all contexts.
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

  const Triple& triple() const
  { return m_Triple; }

  static const char* version();

private:
  // -----  General Options  ----- //
  GeneralOptions m_Options;
  InputTree *m_pInputTree;
  Input* m_pBitcode;
  Output* m_pOutput;
  llvm::Triple m_Triple;

  // -----  factories  ----- //
  InputFactory *m_pInputFactory;
  AttributeFactory *m_pAttrFactory;
  ContextFactory *m_pCntxtFactory;
};

} // namespace of mcld

#endif

