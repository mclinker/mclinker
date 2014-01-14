//===- TargetMachine.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_CODEGEN_TARGETMACHINE_H
#define MCLD_CODEGEN_TARGETMACHINE_H
#include <llvm/Support/CodeGen.h>
#include <string>

namespace llvm {

class Target;
class TargetData;
class TargetMachine;
class MCContext;
class raw_ostream;
class formatted_raw_ostream;
namespace legacy {
class PassManagerBase;
} // namepsace legacy
} // namespace llvm

namespace mcld {

class Module;
class Target;
class FileHandle;
class LinkerConfig;
class ToolOutputFile;

enum CodeGenFileType {
  CGFT_ASMFile,
  CGFT_OBJFile,
  CGFT_DSOFile,
  CGFT_EXEFile,
  CGFT_PARTIAL,
  CGFT_BINARY,
  CGFT_NULLFile
};


/** \class mcld::MCLDTargetMachine
 *  \brief mcld::MCLDTargetMachine is a object adapter of LLVMTargetMachine.
 */
class MCLDTargetMachine
{
public:
  /// Adapter of llvm::TargetMachine
  ///
  MCLDTargetMachine(llvm::TargetMachine& pTM,
                    const llvm::Target& pLLMVTarget,
                    const mcld::Target& pMCLDTarget,
                    const std::string& pTriple);

  virtual ~MCLDTargetMachine();

  /// getTarget - adapt llvm::TargetMachine::getTarget
  const mcld::Target& getTarget() const;

  /// getTM - return adapted the llvm::TargetMachine.
  const llvm::TargetMachine& getTM() const { return m_TM; }
  llvm::TargetMachine&       getTM()       { return m_TM; }

  /// appPassesToEmitFile - The target function which we has to modify as
  /// upstreaming.
  bool addPassesToEmitFile(llvm::legacy::PassManagerBase &,
                           mcld::ToolOutputFile& pOutput,
                           mcld::CodeGenFileType,
                           llvm::CodeGenOpt::Level,
                           mcld::Module& pModule,
                           mcld::LinkerConfig& pConfig,
                           bool DisableVerify = true);

private:
  /// addCommonCodeGenPasses - Add standard LLVM codegen passes used for
  /// both emitting to assembly files or machine code output.
  bool addCommonCodeGenPasses(llvm::legacy::PassManagerBase &,
                              mcld::CodeGenFileType,
                              llvm::CodeGenOpt::Level,
                              bool DisableVerify,
                              llvm::MCContext *&OutCtx);

  bool addCompilerPasses(llvm::legacy::PassManagerBase &pPM,
                         llvm::formatted_raw_ostream &pOutput,
                         llvm::MCContext *&OutCtx);

  bool addAssemblerPasses(llvm::legacy::PassManagerBase &pPM,
                          llvm::raw_ostream &pOutput,
                          llvm::MCContext *&OutCtx);

  bool addLinkerPasses(llvm::legacy::PassManagerBase &pPM,
                       mcld::LinkerConfig& pConfig,
                       mcld::Module& pModule,
                       mcld::FileHandle& pFileHandle,
                       llvm::MCContext *&OutCtx);

private:
  llvm::TargetMachine &m_TM;
  const llvm::Target *m_pLLVMTarget;
  const mcld::Target *m_pMCLDTarget;
  const std::string& m_Triple;
};

} // namespace of mcld

#endif

