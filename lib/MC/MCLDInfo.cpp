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
#include <mcld/Config/Config.h>
#include <string>

using namespace mcld;

//==========================
// MCLDInfo
MCLDInfo::MCLDInfo(const std::string& pTripleString,
                   size_t pAttrNum,
                   size_t pInputNum)
  : m_Options(), m_pBitcode(0), m_Triple(pTripleString)
{
  m_pAttrFactory = new AttributeFactory(pAttrNum);
  m_pCntxtFactory = new ContextFactory(pInputNum);
  m_pMemAreaFactory = new MemoryAreaFactory(pInputNum);
  m_pInputFactory = new InputFactory(pInputNum, *m_pAttrFactory);
  m_pInputTree = new InputTree(*m_pInputFactory);
  m_pOutput = new mcld::Output();
}

MCLDInfo::~MCLDInfo()
{
  delete m_pOutput;
  delete m_pAttrFactory;
  delete m_pCntxtFactory;
  delete m_pMemAreaFactory;
  delete m_pInputFactory;
  delete m_pInputTree;
}

void MCLDInfo::setBitcode(const Input& pInput)
{
  m_pBitcode = const_cast<Input*>(&pInput);
}

Input& MCLDInfo::bitcode()
{
  assert((0 != m_pBitcode) && "default bitcode is not set");
  return *m_pBitcode;
}

const Input& MCLDInfo::bitcode() const
{
  assert((0 != m_pBitcode) && "default bitcode is not set");
  return *m_pBitcode;
}

const char* MCLDInfo::version()
{
  return mcld::internal::version;
}
