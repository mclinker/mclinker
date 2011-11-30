/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/LDRelocation.h>


using namespace mcld;

LDRelocation::LDRelocation(const LDHowto& pHowto, 
	                                    MCFragmentRef& pFragmentRef, 
	                                    uint64_t pAddend,
		                                uint32_t pType)
	: m_pHowto(&pHowto), m_FragmentRef(pFragmentRef), m_Addend(pAddend), m_Type(pType){
	
}

LDRelocation::~LDRelocation()
{
}

const ResolveInfo* LDRelocation::symInfo() const
{ return m_pSymInfo; }

uint64_t LDRelocation::addend() const
{ return m_Addend; }
  
uint32_t LDRelocation::type() const
{ return m_Type; }
  
const LDHowto* LDRelocation::howto() const
{ return m_pHowto; }  

uint32_t LDRelocation::place() const
{
}
