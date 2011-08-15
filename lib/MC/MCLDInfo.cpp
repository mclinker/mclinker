/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   csmon7507 <csmon7507@gmail.com>                                         *
 ****************************************************************************/
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>
#include <mcld/MC/ContextFactory.h>
#include <string>

using namespace mcld;

//==========================
// MCLDInfo
MCLDInfo::MCLDInfo(size_t pAttrNum, size_t pInputNum)
  : m_Options()
{
  m_pAttrFactory = new AttributeFactory(pAttrNum);
  m_pCntxtFactory = new ContextFactory(pInputNum);
  m_pInputFactory = new InputFactory(pInputNum, *m_pAttrFactory);
  m_pInputTree = new InputTree(*m_pInputFactory);
}

MCLDInfo::~MCLDInfo()
{
  delete m_pAttrFactory;
  delete m_pCntxtFactory;
  delete m_pInputFactory;
  delete m_pInputTree;
}

