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
#include <llvm/ADT/StringRef.h>
#include <llvm/CodeGen/MachineFunctionPass.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDInfo.h>
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
  public:
    class PositionDependentOption
    {
    public:
      enum Type {
        NAMESPEC,
        START_GROUP,
        END_GROUP,
        INPUT_FILE
      };

    public:
      PositionDependentOption(unsigned int pPosition,
                             Type pType)
        : m_Type(pType), m_Position(pPosition), m_pPath(0), m_pNamespec(0) {
      }

      PositionDependentOption(unsigned int pPosition,
                             const sys::fs::Path& pInputFile,
                             Type pType = INPUT_FILE)
        : m_Type(pType), m_Position(pPosition),
          m_pPath(&pInputFile), m_pNamespec(0) {
      }

      PositionDependentOption(unsigned int pPosition,
                             const sys::fs::Path& pLibrary,
                             llvm::StringRef pNamespec,
                             Type pType = NAMESPEC)
        : m_Type(pType), m_Position(pPosition),
          m_pPath(&pLibrary), m_pNamespec(pNamespec.data()) {
      }

      const Type& type() const {
        return m_Type;
      }

      unsigned int position() const {
        return m_Position;
      }

      const sys::fs::Path* path() const {
        return m_pPath;
      }

      const char* namespec() const {
        return m_pNamespec;
      }

    private:
      Type m_Type;
      unsigned int m_Position;
      const sys::fs::Path *m_pPath;
      const char *m_pNamespec;
    };

    typedef std::vector<PositionDependentOption*> PositionDependentOptions;

  protected:
    SectLinker(MCLDInfo& pLDInfo,
               TargetLDBackend &pLDBackend);

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
    void initializeInputTree(InputTree &pInputTree,
                             const PositionDependentOptions &pOptions) const;

  protected:
    TargetLDBackend *m_pLDBackend;
    MCLDDriver *m_pLDDriver;
    MCLDInfo& m_LDInfo;

  private:
    static char m_ID;
  };

} // namespace of MC Linker

#endif

