/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/LD/ResolveInfoFactory.h>
#include <cstring>
#include <cstdlib>

using namespace mcld;

//==========================
// ResolveInfoFactory
ResolveInfoFactory::entry_type*
ResolveInfoFactory::produce(const ResolveInfoFactory::key_type& pKey)
{
  entry_type* result = static_cast<entry_type*>(
                                    malloc(sizeof(entry_type)+pKey.size()+1));
  if (NULL == result)
    return NULL;

  new (result) entry_type();
  std::memcpy(result->m_Name, pKey.data(), pKey.size());
  result->m_Name[pKey.size()] = '\0';
  result->m_BitField |= ResolveInfo::RESOLVE_MASK;
  result->m_BitField |= (pKey.size() << ResolveInfo::NAME_LENGTH_OFFSET);
  return result;
}

void ResolveInfoFactory::destroy(ResolveInfoFactory::entry_type* pEntry)
{
  if (NULL != pEntry) {
    pEntry->~entry_type();
    free(pEntry);
  }
}

