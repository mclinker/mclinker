/*===- ScriptScanner.ll ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===*/

%{
/* C/C++ Declarations */

#include <mcld/LD/LinkerScript/ScriptScanner.h>

typedef mcld::ScriptParser::token token;
typedef mcld::ScriptParser::token_type token_type;

#define yyterminate() return token::END
#define YY_NO_UNISTD_H

%}

/* Flex Declarations and Options */
%option c++
%option batch
%option yywrap
%option nounput
%option stack

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /* Regular Expressions */

 /* code to place at the beginning of yylex() */
%{
  // reset location
  yylloc->step();
%}

 /* pass all other characters up to bison */
. { return static_cast<token_type>(*yytext); }

%% /* Additional Code */

namespace mcld {

ScriptScanner::ScriptScanner(std::istream* yyin, std::ostream* yyout)
  : yyFlexLexer(yyin, yyout)
{
}

ScriptScanner::~ScriptScanner()
{
}

} /* namespace of mcld */

#ifdef yylex
#undef yylex
#endif

int yyFlexLexer::yylex()
{
  return 0;
}

int yyFlexLexer::yywrap()
{
  return 1;
}

