/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <llvm/ADT/StringSwitch.h>
#include <llvm/MC/MCExpr.h>
#include <llvm/MC/MCValue.h>
#include <llvm/MC/MCFixup.h>
#include <llvm/MC/MCSectionELF.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/raw_ostream.h>
#include "ARMELFObjectReader.h"
#include "../lib/Target/ARM/ARMFixupKinds.h"

using namespace llvm;
using namespace mcld;

//==========================
// Helper functions
static unsigned getRelocTypeInner(const MCValue& Target,
                                  const MCFixup& Fixup,
                                  bool IsPCRel) {
  MCSymbolRefExpr::VariantKind Modifier = Target.isAbsolute() ?
    MCSymbolRefExpr::VK_None : Target.getSymA()->getKind();

  unsigned Type = 0;
  if (IsPCRel) {
    switch ((unsigned)Fixup.getKind()) {
    default: assert(0 && "Unimplemented");
    case FK_Data_4:
      switch (Modifier) {
      default: assert(!"Unsupported Modifier");
      case MCSymbolRefExpr::VK_None:
        Type = ELF::R_ARM_REL32;
        break;
      case MCSymbolRefExpr::VK_ARM_TLSGD:
        assert(0 && "unimplemented");
        break;
      case MCSymbolRefExpr::VK_ARM_GOTTPOFF:
        Type = ELF::R_ARM_TLS_IE32;
        break;
      }
      break;
    case ARM::fixup_arm_uncondbranch:
      switch (Modifier) {
      case MCSymbolRefExpr::VK_ARM_PLT:
        Type = ELF::R_ARM_PLT32;
        break;
      default:
        Type = ELF::R_ARM_CALL;
        break;
      }
      break;
    case ARM::fixup_arm_condbranch:
      Type = ELF::R_ARM_JUMP24;
      break;
    case ARM::fixup_arm_movt_hi16:
    case ARM::fixup_arm_movt_hi16_pcrel:
      Type = ELF::R_ARM_MOVT_PREL;
      break;
    case ARM::fixup_arm_movw_lo16:
    case ARM::fixup_arm_movw_lo16_pcrel:
      Type = ELF::R_ARM_MOVW_PREL_NC;
      break;
    case ARM::fixup_t2_movt_hi16:
    case ARM::fixup_t2_movt_hi16_pcrel:
      Type = ELF::R_ARM_THM_MOVT_PREL;
      break;
    case ARM::fixup_t2_movw_lo16:
    case ARM::fixup_t2_movw_lo16_pcrel:
      Type = ELF::R_ARM_THM_MOVW_PREL_NC;
      break;
    case ARM::fixup_arm_thumb_bl:
    case ARM::fixup_arm_thumb_blx:
      switch (Modifier) {
      case MCSymbolRefExpr::VK_ARM_PLT:
        Type = ELF::R_ARM_THM_CALL;
        break;
      default:
        Type = ELF::R_ARM_NONE;
        break;
      }
      break;
    }
  } else {
    switch ((unsigned)Fixup.getKind()) {
    default: assert(!"invalid fixup kind!");
    case FK_Data_4:
      switch (Modifier) {
      default: assert(!"Unsupported Modifier"); break;
      case MCSymbolRefExpr::VK_ARM_GOT:
        Type = ELF::R_ARM_GOT_BREL;
        break;
      case MCSymbolRefExpr::VK_ARM_TLSGD:
        Type = ELF::R_ARM_TLS_GD32;
        break;
      case MCSymbolRefExpr::VK_ARM_TPOFF:
        Type = ELF::R_ARM_TLS_LE32;
        break;
      case MCSymbolRefExpr::VK_ARM_GOTTPOFF:
        Type = ELF::R_ARM_TLS_IE32;
        break;
      case MCSymbolRefExpr::VK_None:
        Type = ELF::R_ARM_ABS32;
        break;
      case MCSymbolRefExpr::VK_ARM_GOTOFF:
        Type = ELF::R_ARM_GOTOFF32;
        break;
      }
      break;
    case ARM::fixup_arm_ldst_pcrel_12:
    case ARM::fixup_arm_pcrel_10:
    case ARM::fixup_arm_adr_pcrel_12:
    case ARM::fixup_arm_thumb_bl:
    case ARM::fixup_arm_thumb_cb:
    case ARM::fixup_arm_thumb_cp:
    case ARM::fixup_arm_thumb_br:
      assert(0 && "Unimplemented");
      break;
    case ARM::fixup_arm_uncondbranch:
      Type = ELF::R_ARM_CALL;
      break;
    case ARM::fixup_arm_condbranch:
      Type = ELF::R_ARM_JUMP24;
      break;
    case ARM::fixup_arm_movt_hi16:
      Type = ELF::R_ARM_MOVT_ABS;
      break;
    case ARM::fixup_arm_movw_lo16:
      Type = ELF::R_ARM_MOVW_ABS_NC;
      break;
    case ARM::fixup_t2_movt_hi16:
      Type = ELF::R_ARM_THM_MOVT_ABS;
      break;
    case ARM::fixup_t2_movw_lo16:
      Type = ELF::R_ARM_THM_MOVW_ABS_NC;
      break;
    }
  }

  return Type;
}


//==========================
// ARMELFObjectReader
bool ARMELFObjectReader::hasRelocationAddend() {
  return false;
}

unsigned ARMELFObjectReader::getRelocType(const MCValue& Target,
                                          const MCFixup& Fixup,
                                          bool IsPCRel,
                                          bool IsRelocWithSymbol,
                                          int64_t Addend) {
  MCSymbolRefExpr::VariantKind Modifier = Target.isAbsolute() ?
    MCSymbolRefExpr::VK_None : Target.getSymA()->getKind();

  return getRelocTypeInner(Target, Fixup, IsPCRel);
}

const MCSymbol *ARMELFObjectReader::explicitRelSym(const MCAssembler &Asm,
                                                   const MCValue &Target,
                                                   const MCFragment &F,
                                                   const MCFixup &Fixup,
                                                   bool IsPCRel) const {
  const MCSymbol &Symbol = Target.getSymA()->getSymbol();
  bool EmitThisSym = false;

  const MCSectionELF &Section =
    static_cast<const MCSectionELF&>(Symbol.getSection());
  bool InNormalSection = true;
  unsigned RelocType = 0;
  RelocType = getRelocTypeInner(Target, Fixup, IsPCRel);

  // Debug ***************************************************************//
  const MCSymbolRefExpr::VariantKind Kind = Target.getSymA()->getKind();  //
  MCSymbolRefExpr::VariantKind Kind2;                                     //
  Kind2 = Target.getSymB() ?                                              //
      Target.getSymB()->getKind() : MCSymbolRefExpr::VK_None;             //
  errs() << "considering symbol "                                         //
    << Section.getSectionName() << "/"                                    //
    << Symbol.getName() << "/"                                            //
    << " Rel:" << (unsigned)RelocType                                     //
    << " Kind: " << (int)Kind << "/" << (int)Kind2                        //
    << " Tmp:"                                                            //
    << Symbol.isAbsolute() << "/" << Symbol.isDefined() << "/"            //
    << Symbol.isVariable() << "/" << Symbol.isTemporary() << "\n";        //
  //**********************************************************************//

  if (IsPCRel) {
    switch (RelocType) {
    default:
      // Most relocation types are emitted as explicit symbols
      InNormalSection =
        StringSwitch<bool>(Section.getSectionName())
        .Case(".data.rel.ro.local", false)
        .Case(".data.rel", false)
        .Case(".bss", false)
        .Default(true);
      EmitThisSym = true;
      break;
    case ELF::R_ARM_ABS32:
      // But things get strange with R_ARM_ABS32
      // In this case, most things that go in .rodata show up
      // as section relative relocations
      InNormalSection =
        StringSwitch<bool>(Section.getSectionName())
        .Case(".data.rel.ro.local", false)
        .Case(".data.rel", false)
        .Case(".rodata", false)
        .Case(".bss", false)
        .Default(true);
      EmitThisSym = false;
      break;
    }
  } else {
    InNormalSection =
      StringSwitch<bool>(Section.getSectionName())
      .Case(".data.rel.ro.local", false)
      .Case(".rodata", false)
      .Case(".data.rel", false)
      .Case(".bss", false)
      .Default(true);

    switch (RelocType) {
    default: EmitThisSym = true; break;
    case ELF::R_ARM_ABS32: EmitThisSym = false; break;
    }
  }

  if (EmitThisSym)
    return &Symbol;
  if (! Symbol.isTemporary() && InNormalSection) {
    return &Symbol;
  }
  return NULL;
}
