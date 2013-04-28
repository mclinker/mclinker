//===- ScriptScanner.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_SCANNER_H
#define MCLD_SCRIPT_SCANNER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif

#ifndef YY_DECL
#define YY_DECL                                                       \
  mcld::ScriptParser::token_type                                      \
  mcld::ScriptScanner::lex(mcld::ScriptParser::semantic_type* yylval, \
                           mcld::ScriptParser::location_type* yylloc)
#endif

#include "ScriptParser.h"

namespace mcld {

/** \class ScriptScanner
 *
 */
class ScriptScanner : public yyFlexLexer
{
public:
  ScriptScanner(std::istream* yyin = NULL, std::ostream* yyout = NULL);

  virtual ~ScriptScanner();

  virtual ScriptParser::token_type lex(ScriptParser::semantic_type* yylval,
                                       ScriptParser::location_type* yylloc);

private:
  void enterComments(ScriptParser::location_type& pLocation);
};

} // namespace of mcld

#endif

