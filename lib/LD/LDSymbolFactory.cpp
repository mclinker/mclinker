/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/LD/LDSymbolFactory.h>
#include <cstdlib>
#include <cstring>

using namespace mcld;

//==========================
// LDSymbolFactory
LDSymbolFactory::entry_type*
LDSymbolFactory::produce(const LDSymbolFactory::key_type& pKey)
{
  entry_type* result = static_cast<entry_type*>(
                                    malloc(sizeof(entry_type)+pKey.size()+1));
  if (NULL == result)
    return NULL;

  new (result) entry_type();
  std::memcpy(result->m_String, pKey.data(), pKey.size());
  result->m_String[pKey.size()] = '\0';
  return result;
}

void LDSymbolFactory::destroy(LDSymbolFactory::entry_type* pSymbol)
{
  if (NULL != pSymbol)
    free(pSymbol);
}

