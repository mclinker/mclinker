//===- MCLinker.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MCLinker is a base class inherited by target specific linker.
// This class primarily handles common functionality used by all linkers.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTION_LINKER_H
#define MCLD_SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/CodeGen/MachineFunctionPass.h>
#include <mcld/Support/PositionDependentOption.h>
#include <vector>

namespace llvm {

class Module;
class MachineFunction;

} // namespace of llvm

namespace mcld {

class Module;
class LinkerConfig;
class MCLDFile;
class ObjectLinker;
class TargetLDBackend;
class AttributeFactory;
class SectLinkerOption;
class MemoryAreaFactory;

/** \class MCLinker
*  \brief MCLinker provides a linking pass for standard compilation flow
*
*  MCLinker is responded for
*  - provide an interface for target-specific SectLinekr
*  - set up environment for ObjectLinker
*  - control AsmPrinter, make sure AsmPrinter has already prepared
*    all SectionDatas for linking
*
*  MCLinker resolves the absolue paths of input arguments.
*
*  @see MachineFunctionPass ObjectLinker
*/
class MCLinker : public llvm::MachineFunctionPass
{
protected:
  // Constructor. Although MCLinker has only two arguments,
  // TargetMCLinker should handle
  // - enabled attributes
  // - the default attribute
  // - the default link script
  // - the standard symbols
  MCLinker(SectLinkerOption &pOption,
           TargetLDBackend &pLDBackend,
           mcld::Module& pModule);

public:
  virtual ~MCLinker();

  /// doInitialization - Read all parameters and set up the AsmPrinter.
  /// If your pass overrides this, it must make sure to explicitly call
  /// this implementation.
  virtual bool doInitialization(llvm::Module &pM);

  /// doFinalization - Shut down the AsmPrinter, and do really linking.
  /// If you override this in your pass, you must make sure to call it
  /// explicitly.
  virtual bool doFinalization(llvm::Module &pM);

  /// runOnMachineFunction
  /// redirect to AsmPrinter
  virtual bool runOnMachineFunction(llvm::MachineFunction& pMFn);

protected:
  void initializeInputTree(const PositionDependentOptions &pOptions) const;

  void initializeInputOutput(LinkerConfig& pLDInfo);

  MemoryAreaFactory* memAreaFactory()
  { return m_pMemAreaFactory; }

private:
  SectLinkerOption *m_pOption;

protected:
  TargetLDBackend *m_pLDBackend;
  mcld::Module& m_Module;
  ObjectLinker* m_pObjLinker;
  MemoryAreaFactory *m_pMemAreaFactory;

private:
  static char m_ID;
};

} // namespace of MC Linker

#endif

