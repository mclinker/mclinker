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
#include <mcld/Support/MsgHandling.h>
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

/* abbrev. of RE @ref binutils ld/ldlex.l */
FILENAMECHAR1 [_a-zA-Z\/\.\\\$\_\~]
SYMBOLCHARN   [_a-zA-Z\/\.\\\$\_\~0-9]
FILENAMECHAR    [_a-zA-Z0-9\/\.\-\_\+\=\$\:\[\]\\\,\~]
NOCFILENAMECHAR [_a-zA-Z0-9\/\.\-\_\+\$\:\[\]\\\~]
WS [ \t\r]

/* Starte conditions */
%s LDSCRIPT

%% /* Regular Expressions */

 /* code to place at the beginning of yylex() */
%{
  /* reset location */
  yylloc->step();

  /* determine the initial parser state */
  if (m_Kind == ScriptFile::Unknown) {
    m_Kind = pScriptFile.getKind();
    switch (pScriptFile.getKind()) {
    case ScriptFile::LDScript:
      return token::LINKER_SCRIPT;
    case ScriptFile::Expression:
    case ScriptFile::VersionScript:
    case ScriptFile::DynamicList:
    default:
      assert(0 && "Unsupported script type!");
      break;
    }
  }
%}

 /* Entry Point */
<LDSCRIPT>"ENTRY"                      { return token::ENTRY; }
 /* File Commands */
<LDSCRIPT>"GROUP"                      { return token::GROUP; }
<LDSCRIPT>"AS_NEEDED"                  { return token::AS_NEEDED; }
<LDSCRIPT>"SEARCH_DIR"                 { return token::SEARCH_DIR; }
 /* Format Commands */
<LDSCRIPT>"OUTPUT_FORMAT"              { return token::OUTPUT_FORMAT; }
 /* Misc Commands */
<LDSCRIPT>"OUTPUT_ARCH"                { return token::OUTPUT_ARCH; }

 /* String */
<LDSCRIPT>{FILENAMECHAR1}{FILENAMECHAR}* {
  yylval->strToken.text = yytext;
  yylval->strToken.length = yyleng;
  return token::STRING;
}

 /* gobble up C comments */
<LDSCRIPT>"/*" {
  enterComments(*yylloc);
  yylloc->step();
}

 /* gobble up white-spaces */
<LDSCRIPT>{WS}+ {
  yylloc->step();
}

 /* gobble up end-of-lines */
<LDSCRIPT>\n {
  yylloc->lines(1);
  yylloc->step();
}

 /* pass all other characters up to bison */
. { return static_cast<token_type>(*yytext); }

%% /* Additional Code */

namespace mcld {

ScriptScanner::ScriptScanner(std::istream* yyin, std::ostream* yyout)
  : yyFlexLexer(yyin, yyout), m_Kind(ScriptFile::Unknown)
{
}

ScriptScanner::~ScriptScanner()
{
}

void ScriptScanner::enterComments(ScriptParser::location_type& pLocation)
{
  const int start_line = pLocation.begin.line;
  const int start_col  = pLocation.begin.column;

  int ch = 0;

  while (true) {
    ch = yyinput();
    pLocation.columns(1);

    while (ch != '*' && ch != EOF) {
      if (ch == '\n') {
        pLocation.lines(1);
      }

      ch = yyinput();
      pLocation.columns(1);
    }

    if (ch == '*') {
      ch = yyinput();
      pLocation.columns(1);

      while (ch == '*') {
        ch = yyinput();
        pLocation.columns(1);
      }

      if (ch == '/')
        break;
    }

    if (ch == '\n')
      pLocation.lines(1);

    if (ch == EOF) {
      error(diag::err_unterminated_comment) << pLocation.begin.filename
                                            << start_line
                                            << start_col;
      break;
    }
  }
}

void ScriptScanner::setLexState(ScriptFile::Kind pKind)
{
  /* push the state into the top of stach */
  m_StateStack.push(pKind);

  switch (pKind) {
  case ScriptFile::LDScript:
    BEGIN(LDSCRIPT);
    break;
  case ScriptFile::Expression:
    break;
  case ScriptFile::VersionScript:
  case ScriptFile::DynamicList:
  default:
    assert(0 && "Unsupported script type!");
    break;
  }
}

void ScriptScanner::popLexState()
{
  /* pop the last state */
  m_StateStack.pop();

  /* resume the appropriate state */
  if (!m_StateStack.empty()) {
    switch (m_StateStack.top()) {
    case ScriptFile::LDScript:
      BEGIN(LDSCRIPT);
      break;
    case ScriptFile::Expression:
      break;
    case ScriptFile::VersionScript:
    case ScriptFile::DynamicList:
    default:
      assert(0 && "Unsupported script type!");
      break;
    }
  }
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

