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
%error-verbose
%defines
%define namespace "mcld"
%define "parser_class_name" "ScriptParser"
%parse-param { const class LinkerConfig& pConfig }
%parse-param { class ScriptFile& pScriptFile }
%parse-param { class ScriptScanner& pScriptScanner }
%parse-param { class ScriptReader& pScriptReader }

%locations
%start linker_script

%union {
  char* string;
}

%token END 0 /* EOF */
%token <string> STRING

%token GROUP
%token OUTPUT_FORMAT
%token AS_NEEDED

%type <string> string

%{
#include <mcld/LD/LinkerScript/ScriptReader.h>
#include <mcld/LD/LinkerScript/ScriptScanner.h>
#include <mcld/LD/LinkerScript/ScriptFile.h>
#include <mcld/Support/raw_ostream.h>

#undef yylex
#define yylex pScriptScanner.lex

%}

%% /* Grammar Rules */

linker_script : linker_script script_command
              | /* Empty */
              ;

script_command : output_format_command
               | group_command
               ;

output_format_command : OUTPUT_FORMAT '(' STRING ')'
                        { pScriptFile.addOutputFormatCmd($3); }
                      | OUTPUT_FORMAT '(' STRING ',' STRING ',' STRING ')'
                        { pScriptFile.addOutputFormatCmd($3, $5, $7); }
                      ;

group_command : GROUP
                {
                  pScriptFile.addGroupCmd(pScriptReader.getGroupReader(),
                                          pConfig);
                }
                '(' input_list ')'
              ;

input_list : input_list input_node
           | input_list ',' input_node
           | /* Empty */
           ;

input_node : string
             { pScriptFile.addScriptInput($1); }
           | AS_NEEDED
             { pScriptFile.setAsNeeded(true); }
             '(' input_list ')'
             { pScriptFile.setAsNeeded(false); }
           ;

string : STRING
         { $$ = $1; }
       | '"' STRING '"'
         { $$ = $2; }
       ;

%% /* Additional Code */

void mcld::ScriptParser::error(const mcld::ScriptParser::location_type& pLoc,
                               const std::string &pMsg)
{
}

