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
#include <mcld/Script/ScriptReader.h>
#include <mcld/Script/ScriptScanner.h>
#include <mcld/Script/Operand.h>
#include <mcld/Script/Operator.h>
#include <mcld/Script/Assignment.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/MC/MCLDInput.h>

#undef yylex
#define yylex pScriptScanner.lex
%}

%require "2.4"
%skeleton "lalr1.cc"
%defines "ScriptParser.h"
%debug
%error-verbose
%define namespace "mcld"
%define "parser_class_name" "ScriptParser"
%parse-param { const class LinkerConfig& pConfig }
%parse-param { class LinkerScript& pLDScript }
%parse-param { class ScriptFile& pScriptFile }
%parse-param { class ScriptScanner& pScriptScanner }
%parse-param { class GroupReader& pGroupReader}
%lex-param { const class ScriptFile& pScriptFile }

%locations
%initial-action
{
  /* Initialize the initial location. */
  @$.begin.filename = @$.end.filename = &(pScriptFile.name());
}

%start script_file

%code requires {
#include <llvm/Support/DataTypes.h>
}

%union {
  const std::string* strToken;
  uint64_t intToken;
}

%token END 0 /* EOF */
%token <strToken> STRING
%token <intToken> INTEGER

/* Initial states */
%token LINKER_SCRIPT DEFSYM VERSION_SCRIPT DYNAMIC_LIST

/* Entry point */
%token ENTRY
/* File Commands */
%token INCLUDE
%token INPUT
%token GROUP
%token AS_NEEDED
%token OUTPUT
%token SEARCH_DIR
%token STARTUP
/* Format Commands */
%token OUTPUT_FORMAT
%token TARGET
/* Misc Commands */
%token ASSERT
%token EXTERN
%token FORCE_COMMON_ALLOCATION
%token INHIBIT_COMMON_ALLOCATION
%token INSERT
%token NOCROSSREFS
%token OUTPUT_ARCH
%token LD_FEATURE
/* Assignments */
%token HIDDEN
%token PROVIDE
%token PROVIDE_HIDDEN
/* SECTIONS Command */
%token SECTIONS
/* MEMORY Command */
%token MEMORY
/* PHDRS Command */
%token PHDRS
/* Builtin Functions */
%token ABSOLUTE
%token ADDR
%token ALIGN
%token ALIGNOF
%token BLOCK
%token DATA_SEGMENT_ALIGN
%token DATA_SEGMENT_END
%token DATA_SEGMENT_RELRO_END
%token DEFINED
%token LENGTH
%token LOADADDR
%token MAX
%token MIN
%token NEXT
%token ORIGIN
%token SEGMENT_START
%token SIZEOF
%token SIZEOF_HEADERS
%token CONSTANT
/* Symbolic Constants */
%token MAXPAGESIZE
%token COMMONPAGESIZE
/* Input Section Description */
%token EXCLUDE_FILE
%token COMMON
%token KEEP
%token SORT_BY_NAME
%token SORT_BY_ALIGNMENT
/* Output Section Data */
%token BYTE
%token SHORT
%token LONG
%token QUAD
%token SQUAD
%token FILL
/* Output Section Discarding */
%token DISCARD
/* Output Section Keywords */
%token CREATE_OBJECT_SYMBOLS
%token CONSTRUCTORS
/* Output Section Attributes */
/* Output Section Type */
%token NOLOAD
%token DSECT
%token COPY
%token INFO
%token OVERLAY
/* Output Section LMA */
%token AT
/* Forced Input Alignment */
%token SUBALIGN
/* Output Section Constraint */
%token ONLY_IF_RO
%token ONLY_IF_RW
/* Operators are listed top to bottem, in ascending order */
%left ','
%right '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN AND_ASSIGN OR_ASSIGN LS_ASSIGN RS_ASSIGN
%right '?' ':'
%left LOGICAL_OR
%left LOGICAL_AND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' LE '>' GE
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/' '%'
%right UNARY_PLUS UNARY_MINUS '!' '~'

%type <strToken> string
%type <strToken> symbol

%% /* Grammar Rules */

script_file : LINKER_SCRIPT
              { pScriptScanner.setLexState(ScriptFile::LDScript); }
              linker_script
              { pScriptScanner.popLexState(); }
            ;

linker_script : linker_script script_command
              | /* Empty */
              ;

script_command : entry_command
               | output_format_command
               | group_command
               | search_dir_command
               | output_arch_command
               | { pScriptScanner.setLexState(ScriptFile::Expression); }
                 symbol_assignment
                 { pScriptScanner.popLexState(); }
               ;

entry_command : ENTRY '(' STRING ')'
                { pScriptFile.addEntryPoint(*$3, pLDScript); }
              ;

output_format_command : OUTPUT_FORMAT '(' STRING ')'
                        { pScriptFile.addOutputFormatCmd(*$3); }
                      | OUTPUT_FORMAT '(' STRING ',' STRING ',' STRING ')'
                        { pScriptFile.addOutputFormatCmd(*$3, *$5, *$7); }
                      ;

group_command : GROUP
                {
                  pScriptFile.addGroupCmd(pGroupReader, pConfig);
                }
                '(' input_list ')'
              ;

search_dir_command : SEARCH_DIR '(' STRING ')'
                     { pScriptFile.addSearchDirCmd(*$3, pLDScript); }
                   ;

output_arch_command : OUTPUT_ARCH '(' STRING ')'
                      { pScriptFile.addOutputArchCmd(*$3); }
                    ;

input_list : input_list input_node
           | input_list ',' input_node
           | /* Empty */
           ;

input_node : string
             { pScriptFile.addScriptInput(*$1); }
           | AS_NEEDED
             { pScriptFile.setAsNeeded(true); }
             '(' input_list ')'
             { pScriptFile.setAsNeeded(false); }
           ;

symbol_assignment : symbol
                    { pScriptFile.addAssignment(pLDScript, *$1); }
                    '=' exp ';'
                  | symbol ADD_ASSIGN exp ';'
                  | symbol SUB_ASSIGN exp ';'
                  | symbol MUL_ASSIGN exp ';'
                  | symbol DIV_ASSIGN exp ';'
                  | symbol AND_ASSIGN exp ';'
                  | symbol OR_ASSIGN exp ';'
                  | symbol LS_ASSIGN exp ';'
                  | symbol RS_ASSIGN exp ';'
                  | HIDDEN '(' symbol
                    {
                      pScriptFile.addAssignment(pLDScript, *$3,
                                                Assignment::HIDDEN);
                    }
                    '=' exp ')' ';'
                  | PROVIDE '(' symbol
                    {
                      pScriptFile.addAssignment(pLDScript, *$3,
                                                Assignment::PROVIDE);
                    }
                    '=' exp ')' ';'
                  | PROVIDE_HIDDEN '(' symbol
                    {
                      pScriptFile.addAssignment(pLDScript, *$3,
                                                Assignment::PROVIDE_HIDDEN);
                    }
                    '=' exp ')' ';'
                  ;

exp : '(' exp ')'
    | '+' exp %prec UNARY_PLUS
      { pScriptFile.addExprToken(&Operator::create<Operator::UNARY_PLUS>()); }
    | '-' exp %prec UNARY_MINUS
      { pScriptFile.addExprToken(&Operator::create<Operator::UNARY_MINUS>()); }
    | '!' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LOGICAL_NOT>()); }
    | '~' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::BITWISE_NOT>()); }
    | exp '*' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::MUL>()); }
    | exp '/' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::DIV>()); }
    | exp '%' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::MOD>()); }
    | exp '+' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::ADD>()); }
    | exp '-' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::SUB>()); }
    | exp LSHIFT exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LSHIFT>()); }
    | exp RSHIFT exp
      { pScriptFile.addExprToken(&Operator::create<Operator::RSHIFT>()); }
    | exp '<' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LT>()); }
    | exp LE exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LE>()); }
    | exp '>' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::GT>()); }
    | exp GE exp
      { pScriptFile.addExprToken(&Operator::create<Operator::GE>()); }
    | exp EQ exp
      { pScriptFile.addExprToken(&Operator::create<Operator::EQ>()); }
    | exp NE exp
      { pScriptFile.addExprToken(&Operator::create<Operator::NE>()); }
    | exp '&' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::BITWISE_AND>()); }
    | exp '^' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::BITWISE_XOR>()); }
    | exp '|' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::BITWISE_OR>()); }
    | exp LOGICAL_AND exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LOGICAL_AND>()); }
    | exp LOGICAL_OR exp
      { pScriptFile.addExprToken(&Operator::create<Operator::LOGICAL_OR>()); }
    | exp '?' exp ':' exp
      { pScriptFile.addExprToken(&Operator::create<Operator::TERNARY_IF>()); }
    | ABSOLUTE '(' exp ')'
    | ADDR '(' string ')'
    | ALIGN '(' exp ')'
    | ALIGN '(' exp ',' exp ')'
    | ALIGNOF '(' string ')'
    | BLOCK '(' exp ')'
    | DATA_SEGMENT_ALIGN '(' exp ',' exp ')'
    | DATA_SEGMENT_END '(' exp ')'
    | DATA_SEGMENT_RELRO_END '(' exp ',' exp ')'
    | DEFINED '(' string ')'
    | LENGTH '(' string ')'
    | LOADADDR '(' string ')'
    | MAX '(' exp ',' exp ')'
    | MIN '(' exp ',' exp ')'
    | NEXT '(' exp ')'
    | ORIGIN '(' string ')'
    | SEGMENT_START '(' string ',' exp ')'
    | SIZEOF '(' string ')'
    | SIZEOF_HEADERS
    | CONSTANT '(' MAXPAGESIZE ')'
    | CONSTANT '(' COMMONPAGESIZE')'
    | INTEGER
      { pScriptFile.addExprToken(Operand::create($1)); }
    | symbol
      { pScriptFile.addExprToken(Operand::create(Operand::SYMBOL, *$1)); }
    ;

symbol : STRING
         { $$ = $1; }
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
  position last = pLoc.end - 1;
  std::string filename = "NaN";
  if (last.filename != NULL)
    filename = *last.filename;

  mcld::error(diag::err_syntax_error)
    << filename << last.line << last.column << pMsg;
}

