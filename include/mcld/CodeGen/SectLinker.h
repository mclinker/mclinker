//===- SectLinker.h -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SECTION_LINKER_H
#define SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/MC/MCLDInfo.h"
#include <vector>

namespace llvm
{
  class Module;
  class MachineFunction;
} // namespace of llvm

namespace mcld
{
  class MCLDFile;
  class MCLinker;
  class TargetLDBackend;
  class AttributeFactory;

  /** \class SectLinker
   *  \brief SectLinker provides a linking pass for standard compilation flow
   *
   *  SectLinker is responded for
   *  - provide an interface for target-specific SectLinekr
   *  - collect all target-independent parameters, and set up environment for
   *    MCLinker
   *  - control AsmPrinter, make sure AsmPrinter has already prepared 
   *    all MCSectionDatas for linking
   *
   *  SectLinker resolves the absolue paths of input arguments.
   *
   *  @see MachineFunctionPass MCLinker
   */
  class SectLinker : public llvm::MachineFunctionPass
  {
  public:

    /** \class SectLinker::PositionDependentOption
     *  \brief PositionDependentOptions converts LLVM options into MCLDInfo
     */
    class PositionDependentOption
    {
    public:
      enum Type {
        NAMESPEC,
        INPUT_FILE,
        START_GROUP,
        END_GROUP,
        WHOLE_ARCHIVE,
        NO_WHOLE_ARCHIVE,
        AS_NEEDED,
        NO_AS_NEEDED,
        ADD_NEEDED,
        NO_ADD_NEEDED,
        BDYNAMIC,
        BSTATIC
      };

    public:
      PositionDependentOption(unsigned int pPosition,
                              Type pType);

      explicit PositionDependentOption(unsigned int pPosition,
                             const sys::fs::Path& pInputFile,
                             Type pType = INPUT_FILE);

      explicit PositionDependentOption(unsigned int pPosition,
                             const std::string& pNamespec,
                             Type pType = NAMESPEC);

      const Type& type() const
      { return m_Type; }

      unsigned int position() const
      { return m_Position; }

      const sys::fs::Path* path() const
      { return m_pPath; }

      const std::string& namespec() const
      { return m_pNamespec; }

    private:
      Type m_Type;
      unsigned int m_Position;
      const sys::fs::Path *m_pPath;
      std::string m_pNamespec;
    };

    typedef std::vector<PositionDependentOption*> PositionDependentOptions;

  protected:
    // Constructor. Although SectLinker has only two arguments, 
    // TargetSectLinker should handle
    // - enabled attributes
    // - the default attribute
    // - the default link script
    // - the standard symbols
    //
    // SectLinker constructor handles
    // - the default input
    // - the default output (filename and link type)
    SectLinker(const std::string& pInputFile,
               const std::string& pOutputFile,
               unsigned int pOutputLinkType,
               MCLDInfo& pLDInfo,
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
    void initializeInputTree(MCLDInfo& pLDInfo,
                             const PositionDependentOptions &pOptions) const;

    AttributeFactory* attrFactory()
    { return m_pAttrFactory; }

  protected:
    TargetLDBackend *m_pLDBackend;
    MCLinker *m_pLinker;
    MCLDInfo& m_LDInfo;
    AttributeFactory *m_pAttrFactory;

  private:
    static char m_ID;
  };

} // namespace of MC Linker

#endif

