/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef SECTION_LINKER_H
#define SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/CodeGen/MachineFunctionPass.h>

namespace llvm
{
  class Module;
  class MachineFunction;
} // namespace of llvm

namespace mcld
{
  class MCLDInfo;
  class MCLDFile;
  class MCLDDriver;
  class TargetLDBackend;

  /** \class SectLinker
   *  \brief SectLinker provides a linking pass for standard compilation flow
   *
   *  SectLinker is responded for
   *  - provide an interface for target-specific SectLinekr
   *  - collect all target-independent parameters, and set up environment for
   *    MCLDDriver
   *  - control AsmPrinter, make sure AsmPrinter has already prepared 
   *    all MCSectionDatas for linking
   *
   *  SectLinker resolves the absolue paths of input arguments.
   *
   *  \see MachineFunctionPass MCLDDriver
   */
  class SectLinker : public llvm::MachineFunctionPass
  {
  protected:
    SectLinker(TargetLDBackend &pLDBackend, MCLDFile* pDefaultBitcode = 0);
    virtual MCLDInfo* createLDInfo() const = 0;

  public:
    virtual ~SectLinker();

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
    TargetLDBackend *m_pLDBackend;
    MCLDDriver *m_pLDDriver;
    MCLDInfo* m_pInfo;
    MCLDFile* m_pDefaultBitcode;

  private:
    static char m_ID;
  };

} // namespace of MC Linker

#endif

