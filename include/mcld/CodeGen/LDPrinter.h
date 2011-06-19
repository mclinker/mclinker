/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDPRINTER_H
#define LDPRINTER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/CodeGen/MachineFunctionPass.h>
#include <llvm/Support/DataTypes.h>

namespace llvm
{
  class Module;
  class MachineFunction;
} // namespace of llvm

namespace mcld
{

  using namespace llvm;
  class MCLDScriptReader;
  class MCLDInfo;

  /** \class LDPrinter
   *  \brief LDPrinter provides a pass to link object files.
   *
   *  \see
   *  \author Jush Lu <jush.msn@gmail.com>
   */
  class LDPrinter : public MachineFunctionPass {
  protected:
    LDPrinter();

  public:
    virtual ~LDPrinter();

    /// doInitialization - Set up the AsmPrinter when we are working on a new
    /// module.  If your pass overrides this, it must make sure to explicitly
    /// call this implementation.
    bool doInitialization(Module &pM);

    /// doFinalization - Shut down the asmprinter.  If you override this in your
    /// pass, you must make sure to call it explicitly.
    bool doFinalization(Module &pM);

    /// runOnMachineFunction
    virtual bool runOnMachineFunction( MachineFunction& pF );

  private:
    MCLDInfo* m_pLDInfo;
    MCLDScriptReader *m_pScriptReader;
  };

} // namespace of BOLD

#endif

