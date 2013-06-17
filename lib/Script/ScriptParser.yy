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
#include <mcld/Script/RpnExpr.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/MC/MCLDInput.h>

#undef yylex
#define yylex m_ScriptScanner.lex
%}

%pure-parser
%require "2.4"
%skeleton "lalr1.cc"
%defines "ScriptParser.h"
%debug
%error-verbose
%define namespace "mcld"
%define "parser_class_name" "ScriptParser"
%parse-param { const class LinkerConfig& m_Config }
%parse-param { class LinkerScript& m_LinkerScript }
%parse-param { class ScriptFile& m_ScriptFile }
%parse-param { class ScriptScanner& m_ScriptScanner }
%parse-param { class GroupReader& m_GroupReader}
%parse-param { class RpnExpr* m_pRpnExpr }
%lex-param { const class ScriptFile& m_ScriptFile }

%locations
%initial-action
{
  /* Initialize the initial location. */
  @$.begin.filename = @$.end.filename = &(m_ScriptFile.name());
}

%start script_file

%code requires {
#include <llvm/Support/DataTypes.h>
}

%union {
  const std::string* string;
  uint64_t integer;
  RpnExpr* rpn_expr;
}

%token END 0 /* EOF */
%token <string> STRING LNAMESPEC
%token <integer> INTEGER

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

%type <string> string symbol
%type <rpn_expr> script_exp

%%

script_file : LINKER_SCRIPT
              { m_ScriptScanner.setLexState(ScriptFile::LDScript); }
              linker_script
              { m_ScriptScanner.popLexState(); }
            ;

linker_script : linker_script script_command
              | /* Empty */
              ;

script_command : entry_command
               | output_format_command
               | group_command
               | output_command
               | search_dir_command
               | output_arch_command
               | assert_command
               | symbol_assignment
               | sections_command
               ;

entry_command : ENTRY '(' STRING ')'
                { m_ScriptFile.addEntryPoint(*$3, m_LinkerScript); }
              ;

output_format_command : OUTPUT_FORMAT '(' STRING ')'
                        { m_ScriptFile.addOutputFormatCmd(*$3); }
                      | OUTPUT_FORMAT '(' STRING ',' STRING ',' STRING ')'
                        { m_ScriptFile.addOutputFormatCmd(*$3, *$5, *$7); }
                      ;

group_command : GROUP
                {
                  m_ScriptFile.addGroupCmd(m_GroupReader, m_Config);
                }
                '(' input_list ')'
              ;

search_dir_command : SEARCH_DIR '(' STRING ')' opt_comma
                     { m_ScriptFile.addSearchDirCmd(*$3, m_LinkerScript); }
                   ;

output_command : OUTPUT '(' STRING ')'
                 { m_ScriptFile.addOutputCmd(*$3, m_LinkerScript); }
               ;

output_arch_command : OUTPUT_ARCH '(' STRING ')'
                      { m_ScriptFile.addOutputArchCmd(*$3); }
                    ;

assert_command : ASSERT '(' script_exp ',' string ')'
               ;

input_list : input_list input_node
           | input_list ',' input_node
           | /* Empty */
           ;

input_node : string
             { m_ScriptFile.addScriptInput(*$1); }
           | AS_NEEDED
             { m_ScriptFile.setAsNeeded(true); }
             '(' input_list ')'
             { m_ScriptFile.setAsNeeded(false); }
           ;

/*
  SECTIONS
  {
    sections-command
    sections-command
    ...
  }
*/
sections_command : SECTIONS
                   '{' sect_commands '}'
                 ;

sect_commands : sect_commands sect_cmd
              | /* Empty */
              ;

/*
Each sections-command may of be one of the following:

an ENTRY command (see Entry command)
a symbol assignment (see Assignments)
an output section description
an overlay description
*/
sect_cmd : entry_command
         | symbol_assignment
         | output_sect_desc
         ;

/*
The full description of an output section looks like this:

  section [address] [(type)] :
    [AT(lma)]
    [ALIGN(section_align)]
    [SUBALIGN(subsection_align)]
    [constraint]
    {
      output-section-command
      output-section-command
      ...
    } [>region] [AT>lma_region] [:phdr :phdr ...] [=fillexp]
*/
output_sect_desc : string output_desc_prolog
                   '{'
                       output_sect_commands
                   '}' output_desc_epilog
                 ;

output_desc_prolog : opt_vma_and_type
                     ':'
                     opt_lma opt_align opt_subalign opt_constraint
                   ;

output_sect_commands : output_sect_commands output_sect_cmd
                     | /* Empty */
                     ;

output_desc_epilog : opt_region opt_lma_region opt_phdr opt_fill
                   ;

/* Output Section Attributes */
opt_vma_and_type : exp opt_type
                 | opt_type
                 ;

opt_type : '(' type ')'
         | '(' ')'
         | /* Empty */
         ;

type : NOLOAD
     | DSECT
     | COPY
     | INFO
     | OVERLAY
     ;

opt_lma : AT '(' script_exp ')'
        | /* Empty */
        ;

/* Forced Output Alignment */
opt_align : ALIGN '(' script_exp ')'
          | /* Empty */
          ;

/* Forced Input Alignment */
opt_subalign : SUBALIGN '(' script_exp ')'
             | /* Empty */
             ;

opt_constraint : ONLY_IF_RO
               | ONLY_IF_RW
               | /* Empty */
               ;

opt_region : '>' string
           | /* Empty */
           ;

opt_lma_region : AT '>' string
               | /* Empty */
               ;

opt_phdr : phdrs
         ;

phdrs : phdrs ':' phdr
      | /* Empty */
      ;

phdr : string
     ;

opt_fill : '=' script_exp
         | /* Empty */
         ;

/*
Each output-section-command may be one of the following:

a symbol assignment (see Assignments)
an input section description (see Input Section)
data values to include directly (see Output Section Data)
a special output section keyword (see Output Section Keywords)
*/
output_sect_cmd : symbol_assignment
                | input_sect_desc
                | output_sect_data
                | output_sect_keyword
                ;

input_sect_desc : input_sect_spec
                | KEEP '(' input_sect_spec ')'
                ;

input_sect_spec : string
                | wildcard_file '(' opt_exclude_files input_sect_wildcard_patterns ')'
                ;

wildcard_file : wildcard_pattern
              | SORT_BY_NAME '(' wildcard_pattern ')'
              ;

wildcard_pattern : string
                 | '*'
                 | '?'
                 ;

opt_exclude_files : EXCLUDE_FILE '('
                    exclude_files ')'
                  | /* Empty */
                  ;

exclude_files : exclude_files wildcard_pattern
              | wildcard_pattern
              ;

input_sect_wildcard_patterns : wildcard_sections
                             ;

wildcard_sections : wildcard_sections wildcard_section
                  | wildcard_section
                  ;

wildcard_section : wildcard_pattern
                 | SORT_BY_NAME '(' wildcard_pattern ')'
                 | SORT_BY_ALIGNMENT '(' wildcard_pattern ')'
                 | SORT_BY_NAME '(' SORT_BY_ALIGNMENT '(' wildcard_pattern ')' ')'
                 | SORT_BY_ALIGNMENT '('SORT_BY_NAME '(' wildcard_pattern ')' ')'
                 | SORT_BY_NAME '(' SORT_BY_NAME '(' wildcard_pattern ')' ')'
                 | SORT_BY_ALIGNMENT '(' SORT_BY_ALIGNMENT '(' wildcard_pattern ')' ')'
                 ;

output_sect_data : BYTE  '(' script_exp ')'
                 | SHORT '(' script_exp ')'
                 | LONG  '(' script_exp ')'
                 | QUAD  '(' script_exp ')'
                 | SQUAD '(' script_exp ')'
                 ;

output_sect_keyword : CREATE_OBJECT_SYMBOLS
                    | CONSTRUCTORS
                    | SORT_BY_NAME '(' CONSTRUCTORS ')'
                    ;

symbol_assignment : symbol '=' script_exp ';'
                    { m_ScriptFile.addAssignment(m_LinkerScript, *$1, *$3); }
                  | symbol ADD_ASSIGN exp ';'
                  | symbol SUB_ASSIGN exp ';'
                  | symbol MUL_ASSIGN exp ';'
                  | symbol DIV_ASSIGN exp ';'
                  | symbol AND_ASSIGN exp ';'
                  | symbol OR_ASSIGN exp ';'
                  | symbol LS_ASSIGN exp ';'
                  | symbol RS_ASSIGN exp ';'
                  | HIDDEN '(' symbol '=' script_exp ')' ';'
                    {
                      m_ScriptFile.addAssignment(m_LinkerScript, *$3, *$5,
                                                 Assignment::HIDDEN);
                    }
                  | PROVIDE '(' symbol '=' script_exp ')' ';'
                    {
                      m_ScriptFile.addAssignment(m_LinkerScript, *$3, *$5,
                                                 Assignment::PROVIDE);
                    }
                  | PROVIDE_HIDDEN '(' symbol '=' script_exp ')' ';'
                    {
                      m_ScriptFile.addAssignment(m_LinkerScript, *$3, *$5,
                                                 Assignment::PROVIDE_HIDDEN);
                    }
                  ;

script_exp : {
               m_ScriptScanner.setLexState(ScriptFile::Expression);
               m_pRpnExpr = RpnExpr::create();
             }
             exp
             {
               m_ScriptScanner.popLexState();
               $$ = m_pRpnExpr;
             }
           ;

exp : '(' exp ')'
    | '+' exp %prec UNARY_PLUS
      { m_pRpnExpr->push_back(&Operator::create<Operator::UNARY_PLUS>()); }
    | '-' exp %prec UNARY_MINUS
      { m_pRpnExpr->push_back(&Operator::create<Operator::UNARY_MINUS>()); }
    | '!' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LOGICAL_NOT>()); }
    | '~' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::BITWISE_NOT>()); }
    | exp '*' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::MUL>()); }
    | exp '/' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::DIV>()); }
    | exp '%' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::MOD>()); }
    | exp '+' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::ADD>()); }
    | exp '-' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::SUB>()); }
    | exp LSHIFT exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LSHIFT>()); }
    | exp RSHIFT exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::RSHIFT>()); }
    | exp '<' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LT>()); }
    | exp LE exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LE>()); }
    | exp '>' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::GT>()); }
    | exp GE exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::GE>()); }
    | exp EQ exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::EQ>()); }
    | exp NE exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::NE>()); }
    | exp '&' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::BITWISE_AND>()); }
    | exp '^' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::BITWISE_XOR>()); }
    | exp '|' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::BITWISE_OR>()); }
    | exp LOGICAL_AND exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LOGICAL_AND>()); }
    | exp LOGICAL_OR exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::LOGICAL_OR>()); }
    | exp '?' exp ':' exp
      { m_pRpnExpr->push_back(&Operator::create<Operator::TERNARY_IF>()); }
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
      { m_pRpnExpr->push_back(Operand::create($1)); }
    | symbol
      { m_pRpnExpr->push_back(Operand::create(Operand::SYMBOL, *$1)); }
    ;

symbol : STRING
         { $$ = $1; }
       ;

string : STRING
         { $$ = $1; }
       | '"' STRING '"'
         { $$ = $2; }
       ;

opt_comma : ';'
          | /* Empty */
          ;

%%

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

