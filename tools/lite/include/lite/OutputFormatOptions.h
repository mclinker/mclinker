//===- OutputFormatOptions.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_OUTPUT_FORMAT_OPTIONS_H
#define MCLD_LDLITE_OUTPUT_FORMAT_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/LinkerConfig.h>
#include <mcld/GeneralOptions.h>
#include <mcld/Support/Path.h>
#include <mcld/MC/Input.h>
#include <string>

namespace mcld {

class Module;

class OutputFormatOptions
{
public:
  OutputFormatOptions();

  bool parse(Module& pModule, LinkerConfig& pConfig);

  bool parseOutput(Module& pModule, LinkerConfig& pConfig);

private:
  llvm::cl::opt<mcld::sys::fs::Path,
                false,
                llvm::cl::parser<mcld::sys::fs::Path> >& m_OutputFilename;
  llvm::cl::opt<mcld::LinkerConfig::CodeGenType>& m_FileType;
  llvm::cl::opt<mcld::LinkerConfig::CodeGenType>& m_OFormat;
  llvm::cl::opt<bool>& m_Shared;
  llvm::cl::opt<bool>& m_PIE;
  llvm::cl::opt<bool>& m_Relocatable;
  llvm::cl::opt<mcld::Input::Type>& m_Format;
  llvm::cl::opt<bool>& m_StripDebug;
  llvm::cl::opt<bool>& m_StripAll;
  llvm::cl::opt<bool>& m_DiscardAll;
  llvm::cl::opt<bool>& m_DiscardLocals;
  llvm::cl::opt<bool>& m_EhFrameHdr;
  llvm::cl::opt<bool>& m_NMagic;
  llvm::cl::opt<bool>& m_OMagic;
  llvm::cl::opt<mcld::GeneralOptions::HashStyle>& m_HashStyle;

  llvm::cl::opt<bool>& m_ExportDynamic;
  llvm::cl::opt<std::string>& m_BuildID;
  llvm::cl::list<std::string>& m_ExcludeLIBS;

  llvm::cl::opt<bool>& m_NoWarnMismatch;
};

} // namespace of mcld

#endif

