//===- ELFAttribute.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/ELFAttribute.h>

#include <mcld/ADT/SizeTraits.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LinkerConfig.h>
#include <mcld/MC/Input.h>
#include <mcld/Support/LEB128.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Target/ELFAttributeValue.h>
#include <mcld/Target/GNULDBackend.h>

#include <llvm/ADT/STLExtras.h>
#include <llvm/Support/Host.h>

#include <cstring>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFAttribute
//===----------------------------------------------------------------------===//
ELFAttribute::~ELFAttribute()
{
  llvm::DeleteContainerPointers(m_Subsections);
  return;
}

bool ELFAttribute::merge(const Input &pInput, LDSection &pInputAttrSectHdr)
{
  // Skip corrupt subsection
  if (pInputAttrSectHdr.size() < MinimalELFAttributeSectionSize)
    return true;

  // Obtain the region containing the attribute data. Expect exactly one
  // RegionFragment in the section data.
  const SectionData* sect_data = pInputAttrSectHdr.getSectionData();

  // FIXME: Why is 2?
  if ((sect_data->size() != 2) ||
      (!llvm::isa<RegionFragment>(sect_data->front()))) {
    return true;
  }

  const RegionFragment& region_frag =
      llvm::cast<RegionFragment>(sect_data->front());

  const MemoryRegion& region = region_frag.getRegion();

  // Parse the ELF attribute section header. ARM [ABI-addenda], 2.2.3.
  //
  // <format-version: ‘A’>
  // [ <uint32: subsection-length> NTBS: vendor-name
  //   <bytes: vendor-data>
  // ]*
  const char *attribute_data = reinterpret_cast<const char*>(region.start());

  // format-version
  if (attribute_data[0] != FormatVersion) {
    llvm::errs() << "WARNING: unsupported ELF attribute section format "
                    "(version: " << attribute_data[0] << ")!\n";
    return true;
  }

  size_t subsection_offset = FormatVersionFieldSize;

  // Iterate all subsections containing in this attribute section.
  do {
    const char *subsection_data =
        reinterpret_cast<const char*>(region.getBuffer(subsection_offset));

    // subsection-length
    uint32_t subsection_length =
        *reinterpret_cast<const uint32_t*>(subsection_data);

    if(llvm::sys::IsLittleEndianHost != m_Config.targets().isLittleEndian())
      bswap32(subsection_length);

    // vendor-name
    const char* vendor_name = subsection_data + SubsectionLengthFieldSize;
    const size_t vendor_name_length = ::strlen(vendor_name) + 1 /* '\0' */;

    // Check the length.
    if ((vendor_name_length <= 1) ||
        (subsection_length <= (SubsectionLengthFieldSize + vendor_name_length)))
      return true;

    // Select the attribute subsection.
    Subsection *subsection = getSubsection(vendor_name);

    // Only process the subsections whose vendor can be recognized.
    if (subsection == NULL) {
      llvm::errs() << "WARNING: skip unrecognized private vendor subsection "
                      "(name: " << vendor_name << ")!\n";
    } else {
      // vendor-data
      size_t vendor_data_offset = subsection_offset +
                                  SubsectionLengthFieldSize +
                                  vendor_name_length;
      size_t vendor_data_size = subsection_length - SubsectionLengthFieldSize -
                                vendor_name_length;

      MemoryRegion::ConstAddress vendor_data =
          region.getBuffer(vendor_data_offset);

      // Merge the vendor data in the subsection.
      if (!subsection->merge(pInput, vendor_data, vendor_data_size))
        return false;
    }

    subsection_offset += subsection_length;
  } while ((subsection_offset + SubsectionLengthFieldSize) < pInputAttrSectHdr.size());

  return true;
}

void ELFAttribute::registerAttributeData(ELFAttributeData& pAttrData)
{
  assert((getSubsection(pAttrData.getVendorName()) == NULL) &&
         "Multiple attribute data for a vendor!");
  m_Subsections.push_back(new Subsection(*this, pAttrData));
  return;
}

ELFAttribute::Subsection *
ELFAttribute::getSubsection(llvm::StringRef pVendorName) const
{
  // Search m_Subsections linearly.
  for (llvm::SmallVectorImpl<Subsection*>::const_iterator
          subsec_it = m_Subsections.begin(), subsec_end = m_Subsections.end();
       subsec_it != subsec_end; ++subsec_it) {
    Subsection* const subsection = *subsec_it;
    if (subsection->isMyAttribute(pVendorName)) {
      return subsection;
    }
  }

  // Not found
  return NULL;
}

//===----------------------------------------------------------------------===//
// ELFAttribute::Subsection
//===----------------------------------------------------------------------===//
bool ELFAttribute::Subsection::merge(const Input &pInput,
                                     MemoryRegion::ConstAddress pData,
                                     size_t pSize)
{
  const bool need_swap = (llvm::sys::IsLittleEndianHost !=
                              m_Parent.config().targets().isLittleEndian());
  // Read attribute sub-subsection from vendor data.
  //
  // ARM [ABI-addenda], 2.2.4:
  //
  // [   Tag_File    (=1) <uint32: byte-size> <attribute>*
  //   | Tag_Section (=2) <uint32: byte-size> <section number>* 0 <attribute>*
  //   | Tag_symbol  (=3) <unit32: byte-size> <symbol number>* 0 <attribute>*
  // ] +
  const char *subsubsection_data = reinterpret_cast<const char*>(pData);
  size_t remaining_size = pSize;

  if (!m_AttrData.preMerge(pInput)) {
    return false;
  }

  while (remaining_size > ELFAttribute::MinimalELFAttributeSubsectionSize) {
    // The tag of sub-subsection is encoded in ULEB128.
    size_t tag_size;
    uint64_t tag = leb128::decode<uint64_t>(subsubsection_data, tag_size);

    if ((tag_size + 4 /* byte-size */) >= remaining_size)
      break;

    size_t subsubsection_length =
        *reinterpret_cast<const uint32_t*>(subsubsection_data + tag_size);

    if (need_swap)
      bswap32(subsubsection_length);

    if (subsubsection_length > remaining_size) {
      // The subsubsection is corrupted. Try our best to process it.
      subsubsection_length = remaining_size;
    }

    switch (tag) {
      case ELFAttributeData::Tag_File: {
        ELFAttributeData::TagType tag;
        ELFAttributeValue in_attr;
        // The offset from the start of sub-subsection that <attribute> located
        size_t attribute_offset = tag_size + 4 /* byte-size */;

        const char* attr_buf = subsubsection_data + attribute_offset;
        size_t attr_size = subsubsection_length - attribute_offset;

        // Read attributes from the stream.
        do {
          if (!ELFAttributeData::ReadTag(tag, attr_buf, attr_size))
            break;

          ELFAttributeValue *out_attr;
          bool is_newly_created;

          llvm::tie(out_attr, is_newly_created) =
              m_AttrData.getOrCreateAttributeValue(tag);

          assert(out_attr != NULL);

          if (is_newly_created) {
            // Directly read the attribute value to the out_attr.
            if (!ELFAttributeData::ReadValue(*out_attr, attr_buf, attr_size))
              break;
          } else {
            // The attribute has been defined previously. Read the attribute
            // to a temporary storage in_attr and perform the merge.
            in_attr.reset();
            in_attr.setType(out_attr->type());

            // Read the attribute value.
            if (!ELFAttributeData::ReadValue(in_attr, attr_buf, attr_size))
              break;

            // Merge if the read attribute value is different than current one
            // in output.
            if ((in_attr != *out_attr) &&
                !m_AttrData.merge(pInput, tag, in_attr)) {
              // Fail to merge the attribute.
              return false;
            }
          }
        } while (attr_size > 0);

        break;
      }
      // Skip sub-subsection tagged with Tag_Section and Tag_Symbol. They are
      // deprecated since ARM [ABI-addenda] r2.09.
      case ELFAttributeData::Tag_Section:
      case ELFAttributeData::Tag_Symbol:
      // Skip any unknown tags.
      default: {
        break;
      }
    }

    // Update subsubsection_data and remaining_size for next.
    subsubsection_data += subsubsection_length;
    remaining_size -= subsubsection_length;
  } // while (remaining_size > ELFAttribute::MinimalELFAttributeSubsectionSize)

  return m_AttrData.postMerge(pInput);
}

