//===- ELFDynamic.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

Entry<32, true>::Entry()
{
  m_Pair.d_tag = 0;
  m_Pair.d_un.d_val = 0;
}

Entry<32, true>::~Entry()
{
}

void Entry<32, true>::setValue(uint64_t pTag, uint64_t pValue)
{
  m_Pair.d_tag = pTag;
  m_Pair.d_un.d_val = pValue;
}

size_t Entry<32, true>::emit(uint8_t* pAddress) const
{
  memcpy(reinterpret_cast<void*>(pAddress),
         reinterpret_cast<const void*>(&m_Pair),
         sizeof(Pair));
  return sizeof(Pair);
}

