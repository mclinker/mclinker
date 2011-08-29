/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <llvm/ADT/StringSwitch.h>
#include <llvm/MC/MCExpr.h>
#include <llvm/MC/MCValue.h>
#include <llvm/MC/MCFixup.h>
#include <llvm/MC/MCSectionELF.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/raw_ostream.h>
#include "X86ELFObjectReader.h"

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace llvm;
using namespace mcld;

//==========================
// Helper functions
