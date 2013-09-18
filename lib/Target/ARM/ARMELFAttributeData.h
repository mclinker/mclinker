//===- ARMELFAttributeData.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_ELF_ATTRIBUTE_DATA_H
#define MCLD_ARM_ELF_ATTRIBUTE_DATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/ELFAttributeData.h>
#include <mcld/Target/ELFAttributeValue.h>

#include <map>

namespace mcld {

/** \class ARMELFAttributeData
 *  \brief ARMELFAttributeData handles public ("aeabi") attributes subsection in
 *  ARM ELF.
 *
 */
class ARMELFAttributeData : public ELFAttributeData {
public:
  enum Tag {
    // 0-3 are generic and are defined in ELFAttributeData.
    Tag_CPU_raw_name                = 4,
    Tag_CPU_name                    = 5,
    Tag_CPU_arch                    = 6,
    Tag_CPU_arch_profile            = 7,
    Tag_ARM_ISA_use                 = 8,
    Tag_THUMB_ISA_use               = 9,
    Tag_FP_arch                     = 10,
    Tag_WMMX_arch                   = 11,
    Tag_Advanced_SIMD_arch          = 12,
    Tag_PCS_config                  = 13,
    Tag_ABI_PCS_R9_use              = 14,
    Tag_ABI_PCS_RW_data             = 15,
    Tag_ABI_PCS_RO_data             = 16,
    Tag_ABI_PCS_GOT_use             = 17,
    Tag_ABI_PCS_wchar_t             = 18,
    Tag_ABI_FP_rounding             = 19,
    Tag_ABI_FP_denormal             = 20,
    Tag_ABI_FP_exceptions           = 21,
    Tag_ABI_FP_user_exceptions      = 22,
    Tag_ABI_FP_number_model         = 23,
    Tag_ABI_align_needed            = 24,
    Tag_ABI_align_preserved         = 25,
    Tag_ABI_enum_size               = 26,
    Tag_ABI_HardFP_use              = 27,
    Tag_ABI_VFP_args                = 28,
    Tag_ABI_WMMX_args               = 29,
    Tag_ABI_optimization_goals      = 30,
    Tag_ABI_FP_optimization_goals   = 31,
    Tag_compatibility               = 32,

    Tag_CPU_unaligned_access        = 34,

    Tag_FP_HP_extension             = 36,

    Tag_ABI_FP_16bit_format         = 38,

    Tag_MPextension_use             = 42,

    Tag_DIV_use                     = 44,

    Tag_nodefaults                  = 64,
    Tag_also_compatible_with        = 65,
    Tag_T2EE_use                    = 66,
    Tag_conformance                 = 67,
    Tag_Virtualization_use          = 68,

    Tag_MPextension_use_legacy      = 70,

    Tag_Max = Tag_MPextension_use_legacy,

    // Alias
    Tag_VFP_arch                    = Tag_FP_arch,
    Tag_ABI_align8_needed           = Tag_ABI_align_needed,
    Tag_ABI_align8_preserved        = Tag_ABI_align_preserved,
    Tag_VFP_HP_extension            = Tag_FP_HP_extension
  };

public:
  // ARM [ABI-addenda], 2.2.2: A public attributes subsection is named aeabi.
  ARMELFAttributeData() : ELFAttributeData("aeabi") { }

public:
  virtual const ELFAttributeValue *getAttributeValue(TagType pTag) const;

  virtual std::pair<ELFAttributeValue*, bool>
      getOrCreateAttributeValue(TagType pTag);

  virtual bool merge(const Input &pInput, TagType pTag,
                     const ELFAttributeValue& pInAttr);

private:
  /// GetAttributeValueType - obtain the value type of the indicated tag.
  static unsigned int GetAttributeValueType(TagType pTag);

private:
  // The storage for known tags which is indexed by the tag
  ELFAttributeValue m_Attrs[Tag_Max + 1];

  // The storage for unknown tags
  typedef std::map<TagType, ELFAttributeValue> UnknownAttrsMap;
  UnknownAttrsMap m_UnknownAttrs;
};

} // namespace of mcld

#endif
