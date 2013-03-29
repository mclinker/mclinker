//===- DefSymParser.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DEFSYM_PARSER_H
#define MCLD_DEFSYM_PARSER_H

#include <mcld/LD/LDSymbol.h>
#include <mcld/Module.h>
#include <mcld/Support/MsgHandling.h>

#include <llvm/ADT/StringRef.h>

namespace mcld {

/** \class DefSymParser
 *  \brief DefSymParser parses --defsym option.
 */
class DefSymParser
{
public:
  DefSymParser(const Module& pModule);

  ~DefSymParser();

  //parse a valid expression and set the value in the second paramter
  bool parse(llvm::StringRef, uint64_t&);

private:
  const Module& m_Module;
};

} // mcld

#endif
