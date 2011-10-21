//===- Resolver.cpp -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/Resolver.h"
#include "mcld/LD/LDSymbol.h"
#include <cassert>

using namespace std;
using namespace mcld;

namespace {

const size_t DEF_BIT = 0;
const size_t REF_BIT = 4;
const size_t COM_BIT = 8;

const size_t NOTDYN_BIT = 0;
const size_t DYN_BIT = 2;

const size_t GLOBAL_BIT = 0;
const size_t WEAK_BIT = 1;

/* FIXME: Is there any weak common symbol? */
/* Should make sure the enumeration is continuous. */
enum SymbolType
{
  DEF     =   DEF_BIT | NOTDYN_BIT | GLOBAL_BIT,
  W_DEF   =   DEF_BIT | NOTDYN_BIT |   WEAK_BIT,
  D_DEF   =   DEF_BIT |    DYN_BIT | GLOBAL_BIT,
  DW_DEF  =   DEF_BIT |    DYN_BIT |   WEAK_BIT,

  REF     =   REF_BIT | NOTDYN_BIT | GLOBAL_BIT,
  W_REF   =   REF_BIT | NOTDYN_BIT |   WEAK_BIT,
  D_REF   =   REF_BIT |    DYN_BIT | GLOBAL_BIT,
  DW_REF  =   REF_BIT |    DYN_BIT |   WEAK_BIT,

  COM     =   COM_BIT | NOTDYN_BIT | GLOBAL_BIT,
  W_COM   =   COM_BIT | NOTDYN_BIT |   WEAK_BIT,
  D_COM   =   COM_BIT |    DYN_BIT | GLOBAL_BIT,
  DW_COM  =   COM_BIT |    DYN_BIT |   WEAK_BIT,

  NUMBER_OF_SYMBOL_TYPE
};

SymbolType LDSymbolToType(const LDSymbol& pSym)
{
  size_t type_bit = 0;
  size_t isdyn_bit = 0;
  size_t binding_bit = 0;
  switch (pSym.type()) {
  default:    assert(0 && "Wrong symbol type.");  break;
  case LDSymbol::Defined:    type_bit = DEF_BIT;  break;
  case LDSymbol::Reference:  type_bit = REF_BIT;  break;
  case LDSymbol::Common:     type_bit = COM_BIT;  break;
  }
  isdyn_bit = pSym.isDyn() ? DYN_BIT : NOTDYN_BIT;
  switch (pSym.binding()) {
  default:    assert(0 && "Wrong symbol binding.");  break;
  case LDSymbol::Global:  binding_bit = GLOBAL_BIT;  break;
  case LDSymbol::Weak:    binding_bit =   WEAK_BIT;  break;
  }
  return (SymbolType)(type_bit | isdyn_bit | binding_bit);
}

} // unnamed namespace

//==========================
// Resolver
Resolver::OverrideAction
mcld::Resolver::shouldOverride(const LDSymbol& pOld, const LDSymbol& pNew)
{
  /* OverrideTransitionFunction[old][new] */
  static const OverrideAction
    OverrideTransitionFunction[NUMBER_OF_SYMBOL_TYPE][NUMBER_OF_SYMBOL_TYPE] =
  {
   /* old\new   DEF   W_DEF  D_DEF  DW_DEF  REF   W_REF  D_REF  DW_REF  COM   W_COM  D_COM  DW_COM */
  {/* DEF    */ ERR_D, OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD_W, ERR_O, OLD_W,       },
  {/* W_DEF  */ NEW  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , NEW  , ERR_O, OLD_W,       },
  {/* D_DEF  */ NEW  , NEW  , OLD  , OLD  , NEW  , NEW  , OLD  , OLD  , NEW  , ERR_O, OLD  ,       },
  {/* DW_DEF */ NEW  , NEW  , OLD  , OLD  , NEW  , NEW  , OLD  , OLD  , NEW  , ERR_O, OLD  ,       },
  {/* REF    */ NEW  , NEW  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , NEW  , ERR_O, NEW  ,       },
  {/* W_REF  */ NEW  , NEW  , OLD  , OLD  , NEW  , OLD  , OLD  , OLD  , NEW  , ERR_O, NEW  ,       },
  {/* D_REF  */ NEW  , NEW  , NEW  , NEW  , NEW  , NEW  , OLD  , OLD  , NEW  , ERR_O, NEW  ,       },
  {/* DW_REF */ NEW  , NEW  , NEW  , NEW  , NEW  , NEW  , OLD  , OLD  , NEW  , ERR_O, NEW  ,       },
  {/* COM    */ NEW_W, OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , OLD_B, ERR_O, OLD_B,       },
  {/* W_COM  */                                                                                    },
  {/* D_COM  */ NEW_W, NEW_W, OLD  , OLD  , OLD  , OLD  , OLD  , OLD  , NEW_B, ERR_O, OLD_B,       },
  {/* DW_COM */                                                                                    }
  };

  SymbolType old_type = LDSymbolToType(pOld);
  SymbolType new_type = LDSymbolToType(pNew);
  // TODO: Compute statistic for how many symbol have been defined?
  return OverrideTransitionFunction[old_type][new_type];
}
