//===- OutputFormatOptions.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/OutputFormatOptions.h>
#include <mcld/Module.h>
#include <mcld/Support/MsgHandling.h>

namespace {

llvm::cl::opt<mcld::sys::fs::Path,
              false,
              llvm::cl::parser<mcld::sys::fs::Path> > ArgOutputFilename("o",
  llvm::cl::desc("Output filename"),
  llvm::cl::value_desc("filename"));

llvm::cl::alias AliasOutputFilename("output",
  llvm::cl::desc("alias for -o"),
  llvm::cl::aliasopt(ArgOutputFilename));

llvm::cl::opt<mcld::LinkerConfig::CodeGenType> ArgFileType("filetype",
  llvm::cl::init(mcld::LinkerConfig::Exec),
  llvm::cl::desc("Choose a file type\n"
                 "(not all types are supported by all targets):"),
  llvm::cl::values(
       clEnumValN(mcld::LinkerConfig::Object, "obj",
                  "Emit a relocatable object ('.o') file"),
       clEnumValN(mcld::LinkerConfig::DynObj, "dso",
                  "Emit an dynamic shared object ('.so') file"),
       clEnumValN(mcld::LinkerConfig::Exec, "exe",
                  "Emit an executable ('.exe') file"),
       clEnumValN(mcld::LinkerConfig::Binary, "bin",
                  "Emit a binary file"),
       clEnumValN(mcld::LinkerConfig::External, "null",
                  "Emit nothing for performance testing"),
       clEnumValEnd));

llvm::cl::opt<mcld::LinkerConfig::CodeGenType> ArgOFormat("oformat",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set output format"),
  llvm::cl::init(mcld::LinkerConfig::Unknown),
  llvm::cl::values(
    clEnumValN(mcld::LinkerConfig::Binary, "binary",
      "generate binary machine code."),
    clEnumValEnd));

llvm::cl::opt<bool> ArgShared("shared",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Create a shared library."),
  llvm::cl::init(false));

llvm::cl::alias ArgSharedAlias("Bshareable",
  llvm::cl::desc("alias for -shared"),
  llvm::cl::aliasopt(ArgShared));

llvm::cl::opt<bool> ArgPIE("pie",
  llvm::cl::desc("Emit a position-independent executable file"),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgRelocatable("relocatable",
  llvm::cl::desc("Generate relocatable output"),
  llvm::cl::init(false));

llvm::cl::alias ArgRelocatableAlias("r",
  llvm::cl::desc("alias for --relocatable"),
  llvm::cl::aliasopt(ArgRelocatable));

llvm::cl::opt<mcld::Input::Type> ArgFormat("b",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set input format"),
  llvm::cl::init(mcld::Input::Unknown),
  llvm::cl::values(
    clEnumValN(mcld::Input::Binary, "binary",
      "read in binary machine code."),
    clEnumValEnd));

llvm::cl::alias ArgFormatAlias("format",
  llvm::cl::desc("alias for -b"),
  llvm::cl::aliasopt(ArgFormat));

llvm::cl::opt<bool> ArgStripDebug("strip-debug",
  llvm::cl::desc("Omit debugger symbol information from the output file."),
  llvm::cl::init(false));

llvm::cl::alias ArgStripDebugAlias("S",
  llvm::cl::desc("alias for --strip-debug"),
  llvm::cl::aliasopt(ArgStripDebug));

llvm::cl::opt<bool> ArgStripAll("strip-all",
  llvm::cl::desc("Omit all symbol information from the output file."),
  llvm::cl::init(false));

llvm::cl::alias ArgStripAllAlias("s",
  llvm::cl::desc("alias for --strip-all"),
  llvm::cl::aliasopt(ArgStripAll));

llvm::cl::opt<bool> ArgDiscardAll("discard-all",
  llvm::cl::desc("Delete all local symbols."),
  llvm::cl::init(false));

llvm::cl::alias ArgDiscardAllAlias("x",
  llvm::cl::desc("alias for --discard-all"),
  llvm::cl::aliasopt(ArgDiscardAll));

llvm::cl::opt<bool> ArgDiscardLocals("discard-locals",
  llvm::cl::desc("Delete all temporary local symbols."),
  llvm::cl::init(false));

llvm::cl::alias ArgDiscardLocalsAlias("X",
  llvm::cl::desc("alias for --discard-locals"),
  llvm::cl::aliasopt(ArgDiscardLocals));

llvm::cl::opt<bool> ArgEhFrameHdr("eh-frame-hdr",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Request creation of \".eh_frame_hdr\" section and\n"
                 "ELF \"PT_GNU_EH_FRAME\" segment header."),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgNMagic("nmagic",
  llvm::cl::desc("Do not page align data"),
  llvm::cl::init(false));

llvm::cl::alias ArgNMagicAlias("n",
  llvm::cl::desc("alias for --nmagic"),
  llvm::cl::aliasopt(ArgNMagic));

llvm::cl::opt<bool> ArgOMagic("omagic",
  llvm::cl::desc("Do not page align data, do not make text readonly"),
  llvm::cl::init(false));

llvm::cl::alias ArgOMagicAlias("N",
  llvm::cl::desc("alias for --omagic"),
  llvm::cl::aliasopt(ArgOMagic));

llvm::cl::opt<mcld::GeneralOptions::HashStyle> ArgHashStyle("hash-style",
  llvm::cl::init(mcld::GeneralOptions::SystemV),
  llvm::cl::desc("Set the type of linker's hash table(s)."),
  llvm::cl::values(
       clEnumValN(mcld::GeneralOptions::SystemV, "sysv",
                 "classic ELF .hash section"),
       clEnumValN(mcld::GeneralOptions::GNU, "gnu",
                 "new style GNU .gnu.hash section"),
       clEnumValN(mcld::GeneralOptions::Both, "both",
                 "both the classic ELF and new style GNU hash tables"),
       clEnumValEnd));

llvm::cl::opt<bool> ArgNoWarnMismatch("no-warn-mismatch",
  llvm::cl::desc("Allow linking together mismatched input files."),
  llvm::cl::init(false));

// Not supported yet {
llvm::cl::opt<bool> ArgExportDynamic("export-dynamic",
  llvm::cl::desc("Export all dynamic symbols"),
  llvm::cl::init(false));

llvm::cl::alias ArgExportDynamicAlias("E",
  llvm::cl::desc("alias for --export-dynamic"),
  llvm::cl::aliasopt(ArgExportDynamic));

llvm::cl::opt<std::string> ArgBuildID("build-id",
  llvm::cl::desc("Request creation of \".note.gnu.build-id\" ELF note section."),
  llvm::cl::value_desc("style"),
  llvm::cl::ValueOptional);

llvm::cl::list<std::string> ArgExcludeLIBS("exclude-libs",
  llvm::cl::CommaSeparated,
  llvm::cl::desc("Exclude libraries from automatic export"),
  llvm::cl::value_desc("lib1,lib2,..."));

// } Not supported yet

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputFormatOptions
//===----------------------------------------------------------------------===//
OutputFormatOptions::OutputFormatOptions()
  : m_OutputFilename(ArgOutputFilename),
    m_FileType(ArgFileType),
    m_OFormat(ArgOFormat),
    m_Shared(ArgShared),
    m_PIE(ArgPIE),
    m_Relocatable(ArgRelocatable),
    m_Format(ArgFormat),
    m_StripDebug(ArgStripDebug),
    m_StripAll(ArgStripAll),
    m_DiscardAll(ArgDiscardAll),
    m_DiscardLocals(ArgDiscardLocals),
    m_EhFrameHdr(ArgEhFrameHdr),
    m_NMagic(ArgNMagic),
    m_OMagic(ArgOMagic),
    m_HashStyle(ArgHashStyle),
    m_ExportDynamic(ArgExportDynamic),
    m_BuildID(ArgBuildID),
    m_ExcludeLIBS(ArgExcludeLIBS),
    m_NoWarnMismatch(ArgNoWarnMismatch) {
}

bool OutputFormatOptions::parse(mcld::Module& pModule, LinkerConfig& pConfig)
{
  if (!parseOutput(pModule, pConfig)) {
    mcld::unreachable(mcld::diag::unrecognized_output_file) << pModule.name();
    return false;
  }

  if (mcld::Input::Binary == m_Format)
    pConfig.options().setBinaryInput();

  pConfig.options().setStripDebug(m_StripDebug || m_StripAll);
  if (m_StripAll)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripAllSymbols);
  else if (m_DiscardAll)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripLocals);
  else if (m_DiscardLocals)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripTemporaries);
  else
    pConfig.options().setStripSymbols(mcld::GeneralOptions::KeepAllSymbols);

  pConfig.options().setEhFrameHdr(m_EhFrameHdr);
  pConfig.options().setPIE(m_PIE);
  pConfig.options().setNMagic(m_NMagic);
  pConfig.options().setOMagic(m_OMagic);
  pConfig.options().setHashStyle(m_HashStyle);
  pConfig.options().setExportDynamic(m_ExportDynamic);

  // --exclude-libs
  llvm::cl::list<std::string>::iterator exclude,
                                        excludeEnd = m_ExcludeLIBS.end();
  for (exclude = m_ExcludeLIBS.begin(); exclude != excludeEnd; ++exclude) {
    pConfig.options().excludeLIBS().insert(*exclude);
  }

  if (m_NoWarnMismatch)
    pConfig.options().setWarnMismatch(false);
  else
    pConfig.options().setWarnMismatch(true);
  // build-id
  // exclude-libs

  return true;
}

/// configure the output filename
bool OutputFormatOptions::parseOutput(Module& pModule, LinkerConfig& pConfig)
{
  if (true == m_Shared || true == m_PIE) {
    // -shared or -pie
    m_FileType = mcld::LinkerConfig::DynObj;
  }
  else if (true == m_Relocatable) {
    // partial linking
    m_FileType = mcld::LinkerConfig::Object;
  }
  else if (mcld::LinkerConfig::Binary == m_OFormat) {
    // binary output
    m_FileType = mcld::LinkerConfig::Binary;
  }

  pConfig.setCodeGenType(m_FileType);

  std::string output_filename(m_OutputFilename.native());

  if (m_OutputFilename.empty()) {

    if (llvm::Triple::Win32 == pConfig.targets().triple().getOS()) {
      output_filename.assign("_out");
      switch (m_FileType) {
        case mcld::LinkerConfig::Object: {
          output_filename += ".obj";
        break;
        }
        case mcld::LinkerConfig::DynObj: {
          output_filename += ".dll";
          break;
        }
        case mcld::LinkerConfig::Exec: {
          output_filename += ".exe";
          break;
        }
        case mcld::LinkerConfig::External:
          break;
        default: {
          return false;
          break;
        }
      } // switch
    }
    else {
      if (mcld::LinkerConfig::Object   == m_FileType ||
          mcld::LinkerConfig::DynObj   == m_FileType ||
          mcld::LinkerConfig::Exec     == m_FileType ||
          mcld::LinkerConfig::External == m_FileType) {
        output_filename.assign("a.out");
      }
      else {
        return false;
      }
    }
  } // end of if empty m_OutputFilename

  pModule.setName(output_filename);
  return true;
}
