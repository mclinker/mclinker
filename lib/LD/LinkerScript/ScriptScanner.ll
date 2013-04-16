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
#include <string>

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

/* abbrev. of re */
FILENAMECHAR1 [_a-zA-Z\/\.\\\$\_\~]
SYMBOLCHARN   [_a-zA-Z\/\.\\\$\_\~0-9]
FILENAMECHAR    [_a-zA-Z0-9\/\.\-\_\+\=\$\:\[\]\\\,\~]
NOCFILENAMECHAR [_a-zA-Z0-9\/\.\-\_\+\$\:\[\]\\\~]
WS [ \t\r]

/* Start states */
%x COMMENT

%% /* Regular Expressions */

 /* code to place at the beginning of yylex() */
%{
  // reset location
  yylloc->step();
%}

 /* Entry Point */
"ENTRY"                      { return token::ENTRY; }
 /* File Commands */
"GROUP"                      { return token::GROUP; }
"AS_NEEDED"                  { return token::AS_NEEDED; }
"SEARCH_DIR"                 { return token::SEARCH_DIR; }
 /* Format Commands */
"OUTPUT_FORMAT"              { return token::OUTPUT_FORMAT; }
 /* Misc Commands */
"OUTPUT_ARCH"                { return token::OUTPUT_ARCH; }

 /* String */
{FILENAMECHAR1}{FILENAMECHAR}* {
  yylval->strToken.text = yytext;
  yylval->strToken.length = yyleng;
  return token::STRING;
}

 /* gobble up C comments */
"/*"                  { BEGIN(COMMENT); }
<COMMENT>[^*\n]*      { /* eat anything that's not a '*' */ }
<COMMENT>"*"+[^*/\n]* { /* eat up '*'s not followed by '/'s */ }
<COMMENT>\n           { yylloc->lines(1); }
<COMMENT>"*"+"/"      { BEGIN(INITIAL); }

 /* gobble up white-spaces */
{WS}+ { yylloc->step(); }

 /* gobble up end-of-lines */
\n {
  yylloc->lines(yyleng);
  yylloc->step();
}

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

