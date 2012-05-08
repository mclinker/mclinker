//===- SectLinker.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//SectLinker is a base class inherited by target specific linker.
//This class primarily handles common functionality used by all linkers.
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

namespace llvm
{
  class Module;
  class MachineFunction;
} // namespace of llvm

namespace mcld
{
  class MCLDFile;
  class MCLDDriver;
  class TargetLDBackend;
  class AttributeFactory;
  class SectLinkerOption;

  /** \class SectLinker
   *  \brief SectLinker provides a linking pass for standard compilation flow
   *
   *  SectLinker is responded for
   *  - provide an interface for target-specific SectLinekr
   *  - set up environment for MCLDDriver
   *  - control AsmPrinter, make sure AsmPrinter has already prepared
   *    all MCSectionDatas for linking
   *
   *  SectLinker resolves the absolue paths of input arguments.
   *
   *  @see MachineFunctionPass MCLDDriver
   */
  class SectLinker : public llvm::MachineFunctionPass
  {
  protected:
    // Constructor. Although SectLinker has only two arguments,
    // TargetSectLinker should handle
    // - enabled attributes
    // - the default attribute
    // - the default link script
    // - the standard symbols
    SectLinker(SectLinkerOption &pOption,
               TargetLDBackend &pLDBackend);

  public:
    virtual ~SectLinker();

    /// addTargetOptions - target SectLinker can hook this function to add
    /// target-specific inputs
    virtual void addTargetOptions(llvm::Module &pM,
                                  SectLinkerOption &pOption)
    { }

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

    AttributeFactory* attrFactory()
    { return m_pAttrFactory; }

  private:
    SectLinkerOption *m_pOption;

  protected:
    TargetLDBackend *m_pLDBackend;
    MCLDDriver *m_pLDDriver;
    AttributeFactory *m_pAttrFactory;

  private:
    static char m_ID;
  };

} // namespace of MC Linker

#endif

