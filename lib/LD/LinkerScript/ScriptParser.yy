/*===- ScriptParser.yy ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===*/

%{
/* C/C++ Declarations */

%}

%require "2.3"
%skeleton "lalr1.cc"
%debug
%defines
%define namespace "mcld"
%define "parser_class_name" "ScriptParser"
%parse-param { class ScriptReader& pScriptReader }
%parse-param { class ScriptScanner& pScriptScanner }
%parse-param { class ScriptFile& pScriptFile }
%parse-param { const class LinkerConfig& pConfig }
%locations
%start linker_script

%union {
  std::string* string;
}

%token END 0 /* EOF */
%token <string> STRING

%{
#include <mcld/LD/LinkerScript/ScriptReader.h>
#include <mcld/LD/LinkerScript/ScriptScanner.h>
#include <mcld/LD/LinkerScript/ScriptFile.h>
#include <mcld/Support/raw_ostream.h>

#undef yylex
#define yylex pScriptScanner.lex

%}

%% /* Grammar Rules */

linker_script :
              ;

%% /* Additional Code */

void mcld::ScriptParser::error(const mcld::ScriptParser::location_type& pLoc,
                               const std::string &pMsg)
{
}

