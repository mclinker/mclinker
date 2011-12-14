//===- MCLinker.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides a number of APIs used by SectLinker.
// These APIs do the things which a linker should do.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_MCLINKER_H
#define MCLD_MCLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDCommand.h>
#include <mcld/LD/StrSymPool.h>
#include <mcld/LD/StaticResolver.h>

namespace mcld {

class MCLDCommand;
class MCLDFile;
class Input;
class Layout;

class TargetLDBackend;
class MCLDInfo;

/** \class MCLinker
 *  \brief MCLinker provides a pass to link object files.
 */
class MCLinker
{
public:
  MCLinker(TargetLDBackend& pBackend,
           MCLDInfo& pLDInfo,
           const Resolver& pResolver = StaticResolver());
  ~MCLinker();

  /// mergeSymbolTable - merge the symbol table and resolve symbols.
  ///   Since in current design, MCLinker resolves symbols when reading symbol
  ///   tables, this function do nothing.
  bool mergeSymbolTable(Input& pInput)
  { return true; }

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_Info;
  StrSymPool m_StrSymPool;
};

} // namespace of mcld

#endif

