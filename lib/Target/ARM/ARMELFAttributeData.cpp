//===- ARMELFAttributeData.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMELFAttributeData.h"

using namespace mcld;

const ELFAttributeValue *ARMELFAttributeData::getAttributeValue(TagType pTag) const
{
  if (pTag <= Tag_Max) {
    const ELFAttributeValue &attr_value = m_Attrs[pTag];

    if (attr_value.isInitialized()) {
      return &attr_value;
    } else {
      // Don't return uninitialized attribute value.
      return NULL;
    }
  } else {
    UnknownAttrsMap::const_iterator attr_it = m_UnknownAttrs.find(pTag);

    if (attr_it == m_UnknownAttrs.end()) {
      return NULL;
    } else {
      return &attr_it->second;
    }
  }
}

std::pair<ELFAttributeValue*, bool>
ARMELFAttributeData::getOrCreateAttributeValue(TagType pTag)
{
  ELFAttributeValue *attr_value = NULL;

  if (pTag <= Tag_Max) {
    attr_value = &m_Attrs[pTag];
  } else {
    // An unknown tag encounterred.
    attr_value = &m_UnknownAttrs[pTag];
  }

  assert(attr_value != NULL);

  // Setup the value type.
  if (!attr_value->isUninitialized()) {
    return std::make_pair(attr_value, false);
  } else {
    attr_value->setType(GetAttributeValueType(pTag));
    return std::make_pair(attr_value, true);
  }
}

unsigned int ARMELFAttributeData::GetAttributeValueType(TagType pTag)
{
  // See ARM [ABI-addenda], 2.2.6.
  switch (pTag) {
    case Tag_compatibility: {
      return (ELFAttributeValue::Int | ELFAttributeValue::String);
    }
    case Tag_nodefaults: {
      return (ELFAttributeValue::Int | ELFAttributeValue::NoDefault);
    }
    case Tag_CPU_raw_name:
    case Tag_CPU_name: {
      return ELFAttributeValue::String;
    }
    default: {
      if (pTag < 32)
        return ELFAttributeValue::Int;
      else
        return ((pTag & 1) ? ELFAttributeValue::String :
                             ELFAttributeValue::Int);
    }
  }
  // unreachable
}

bool ARMELFAttributeData::merge(const Input &pInput, TagType pTag,
                                const ELFAttributeValue& pInAttr)
{
  // TODO: Implement logics for merging input attribtues.
  return false;
}
