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
#include <mcld/Script/FileToken.h>
#include <mcld/Script/NameSpec.h>
#include <mcld/Script/WildcardPattern.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/MC/MCLDInput.h>

#undef yylex
#define yylex m_ScriptScanner.lex
%}

%code requires {
#include <mcld/Script/StrToken.h>
#include <mcld/Script/StringList.h>
#include <mcld/Script/OutputSectDesc.h>
#include <mcld/Script/InputSectDesc.h>
#include <llvm/Support/DataTypes.h>
}

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
%parse-param { class StringList* m_pStringList }
%parse-param { bool m_bAsNeeded }
%lex-param { const class ScriptFile& m_ScriptFile }

%locations
%initial-action
{
  /* Initialize the initial location. */
  @$.begin.filename = @$.end.filename = &(m_ScriptFile.name());
}

%start script_file

%union {
  const std::string* string;
  uint64_t integer;
  RpnExpr* rpn_expr;
  StrToken* str_token;
  StringList* str_tokens;
  OutputSectDesc::Prolog output_prolog;
  OutputSectDesc::Type output_type;
  OutputSectDesc::Constraint output_constraint;
  OutputSectDesc::Epilog output_epilog;
  WildcardPattern* wildcard;
  InputSectDesc::Spec input_spec;
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

%type <string> string symbol opt_region opt_lma_region wildcard_pattern
%type <rpn_expr> script_exp opt_lma opt_align opt_subalign opt_fill
%type <str_token> input phdr
%type <str_tokens> input_list opt_phdr opt_exclude_files input_sect_wildcard_patterns
%type <output_prolog> output_desc_prolog opt_vma_and_type
%type <output_type> opt_type type
%type <output_constraint> opt_constraint
%type <output_epilog> output_desc_epilog
%type <wildcard> wildcard_file wildcard_section
%type <input_spec> input_sect_spec

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

group_command : GROUP '(' input_list ')'
                {
                  m_ScriptFile.addGroupCmd(*$3, m_GroupReader, m_Config,
                                           m_LinkerScript);
                }
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
                 { m_ScriptFile.addAssertCmd(*$3, *$5); }
               ;

input_list : { m_pStringList = StringList::create(); }
             inputs
             { $$ = m_pStringList; }
           ;

inputs : input
         { m_pStringList->push_back($1); }
       | inputs input
         { m_pStringList->push_back($2); }
       | inputs ',' input
         { m_pStringList->push_back($3); }
       | AS_NEEDED '('
         { m_bAsNeeded = true; }
         inputs ')'
         { m_bAsNeeded = false; }
       | inputs AS_NEEDED '('
         { m_bAsNeeded = true; }
         inputs ')'
         { m_bAsNeeded = false; }
       | inputs ',' AS_NEEDED '('
         { m_bAsNeeded = true; }
         inputs ')'
         { m_bAsNeeded = false; }
       ;

input : string
        { $$ = FileToken::create(*$1, m_bAsNeeded); }
      | LNAMESPEC
        { $$ = NameSpec::create(*$1, m_bAsNeeded); }
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
                   { m_ScriptFile.enterSectionsCmd(); }
                   '{' sect_commands '}'
                   { m_ScriptFile.leaveSectionsCmd(); }
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
                   { m_ScriptFile.enterOutputSectDesc(*$1, $2); }
                   '{'
                       output_sect_commands
                   '}' output_desc_epilog
                   { m_ScriptFile.leaveOutputSectDesc($7); }
                 ;

output_desc_prolog : {
                       m_ScriptScanner.setLexState(ScriptFile::Expression);
                       /* create exp for vma */
                       m_pRpnExpr = RpnExpr::create();
                     }
                     opt_vma_and_type
                     { m_ScriptScanner.popLexState(); }
                     ':'
                     opt_lma opt_align opt_subalign opt_constraint
                     {
                       $$.m_pVMA       = $2.m_pVMA;
                       $$.m_Type       = $2.m_Type;
                       $$.m_pLMA       = $5;
                       $$.m_pAlign     = $6;
                       $$.m_pSubAlign  = $7;
                       $$.m_Constraint = $8;
                     }
                   ;

output_sect_commands : output_sect_commands output_sect_cmd
                     | /* Empty */
                     ;

output_desc_epilog : opt_region opt_lma_region opt_phdr opt_fill
                     {
                        $$.m_pRegion    = $1;
                        $$.m_pLMARegion = $2;
                        $$.m_pPhdrs     = $3;
                        $$.m_pFillExp   = $4;
                     }
                   ;

/* Output Section Attributes */
opt_vma_and_type : exp opt_type
                   {
                     $$.m_pVMA = m_pRpnExpr;
                     $$.m_Type = $2;
                   }
                 | opt_type
                   {
                     $$.m_pVMA = NULL;
                     $$.m_Type = $1;
                   }
                 ;

opt_type : '(' type ')'
           { $$ = $2; }
         | '(' ')'
           { $$ = OutputSectDesc::LOAD; }
         | /* Empty */
           { $$ = OutputSectDesc::LOAD; }
         ;

type : NOLOAD
       { $$ = OutputSectDesc::NOLOAD; }
     | DSECT
       { $$ = OutputSectDesc::DSECT; }
     | COPY
       { $$ = OutputSectDesc::COPY; }
     | INFO
       { $$ = OutputSectDesc::INFO; }
     | OVERLAY
       { $$ = OutputSectDesc::OVERLAY; }
     ;

opt_lma : AT '(' script_exp ')'
          { $$ = $3; }
        | /* Empty */
          { $$ = NULL; }
        ;

/* Forced Output Alignment */
opt_align : ALIGN '(' script_exp ')'
            { $$ = $3; }
          | /* Empty */
            { $$ = NULL; }
          ;

/* Forced Input Alignment */
opt_subalign : SUBALIGN '(' script_exp ')'
               { $$ = $3; }
             | /* Empty */
               { $$ = NULL; }
             ;

opt_constraint : ONLY_IF_RO
                 { $$ = OutputSectDesc::ONLY_IF_RO; }
               | ONLY_IF_RW
                 { $$ = OutputSectDesc::ONLY_IF_RW; }
               | /* Empty */
                 { $$ = OutputSectDesc::NO_CONSTRAINT; }
               ;

opt_region : '>' string
             { $$ = $2; }
           | /* Empty */
             { $$ = NULL; }
           ;

opt_lma_region : AT '>' string
                 { $$ = $3; }
               | /* Empty */
                 { $$ = NULL; }
               ;

opt_phdr : { m_pStringList = StringList::create(); }
           phdrs
           { $$ = m_pStringList; }
         ;

phdrs : phdrs ':' phdr
        { m_pStringList->push_back($3); }
      | /* Empty */
      ;

phdr : string
       { $$ = StrToken::create(*$1); }
     ;

opt_fill : '=' script_exp
           { $$ = $2; }
         | /* Empty */
           { $$ = NULL; }
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
                  { m_ScriptFile.addInputSectDesc(InputSectDesc::NoKeep, $1); }
                | KEEP '(' input_sect_spec ')'
                  { m_ScriptFile.addInputSectDesc(InputSectDesc::Keep, $3); }
                ;

input_sect_spec : string
                  {
                    $$.file = WildcardPattern::create(*$1, WildcardPattern::SORT_NONE);
                    $$.exclude_files = NULL;
                    $$.wildcard_sections = NULL;
                  }
                | wildcard_file '(' opt_exclude_files input_sect_wildcard_patterns ')'
                  {
                    $$.file = $1;
                    $$.exclude_files = $3;
                    $$.wildcard_sections = $4;
                  }
                ;

wildcard_file : wildcard_pattern
                { $$ = WildcardPattern::create(*$1, WildcardPattern::SORT_NONE); }
              | SORT_BY_NAME '(' wildcard_pattern ')'
                { $$ = WildcardPattern::create(*$3, WildcardPattern::SORT_BY_NAME); }
              ;

wildcard_pattern : string
                   { $$ = $1; }
                 | '*'
                   { $$ = &m_ScriptFile.createParserStr("*", 1); }
                 | '?'
                   { $$ = &m_ScriptFile.createParserStr("?", 1); }
                 ;

opt_exclude_files : EXCLUDE_FILE '('
                    { m_pStringList = StringList::create(); }
                    exclude_files ')'
                    { $$ = m_pStringList; }
                  | /* Empty */
                    { $$ = NULL; }
                  ;

exclude_files : exclude_files wildcard_pattern
                {
                  m_pStringList->push_back(
                    WildcardPattern::create(*$2, WildcardPattern::SORT_NONE));
                }
              | wildcard_pattern
                {
                  m_pStringList->push_back(
                    WildcardPattern::create(*$1, WildcardPattern::SORT_NONE));
                }
              ;

input_sect_wildcard_patterns : { m_pStringList = StringList::create(); }
                               wildcard_sections
                               { $$ = m_pStringList; }
                             ;

wildcard_sections : wildcard_sections wildcard_section
                    {
                      m_pStringList->push_back($2);
                    }
                  | wildcard_section
                    {
                      m_pStringList->push_back($1);
                    }
                  ;

wildcard_section : wildcard_pattern
                   { $$ = WildcardPattern::create(*$1, WildcardPattern::SORT_NONE); }
                 | SORT_BY_NAME '(' wildcard_pattern ')'
                   { $$ = WildcardPattern::create(*$3, WildcardPattern::SORT_BY_NAME); }
                 | SORT_BY_ALIGNMENT '(' wildcard_pattern ')'
                   { $$ = WildcardPattern::create(*$3, WildcardPattern::SORT_BY_ALIGNMENT); }
                 | SORT_BY_NAME '(' SORT_BY_ALIGNMENT '(' wildcard_pattern ')' ')'
                   { $$ = WildcardPattern::create(*$5, WildcardPattern::SORT_BY_NAME_ALIGNMENT); }
                 | SORT_BY_ALIGNMENT '('SORT_BY_NAME '(' wildcard_pattern ')' ')'
                   { $$ = WildcardPattern::create(*$5, WildcardPattern::SORT_BY_ALIGNMENT_NAME); }
                 | SORT_BY_NAME '(' SORT_BY_NAME '(' wildcard_pattern ')' ')'
                   { $$ = WildcardPattern::create(*$5, WildcardPattern::SORT_BY_NAME); }
                 | SORT_BY_ALIGNMENT '(' SORT_BY_ALIGNMENT '(' wildcard_pattern ')' ')'
                   { $$ = WildcardPattern::create(*$5, WildcardPattern::SORT_BY_ALIGNMENT); }
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
      { m_pRpnExpr->push_back(IntOperand::create($1)); }
    | symbol
      { m_pRpnExpr->push_back(SymOperand::create(*$1)); }
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

