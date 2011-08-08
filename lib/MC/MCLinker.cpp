/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCLDLayout.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/Target/TargetLDBackend.h>

using namespace mcld;

MCLinker::MCLinker( TargetLDBackend& pBackend )
: m_pBackend(pBackend)
{
}

MCLinker::~MCLinker()
{
}

void MCLinker::addCommand( MCLDCommand& pCommand )
{
}

void MCLinker::setLayout( MCLDLayout& pLayout )
{
}

void MCLinker::addLdFile( MCLDFile& pLDFile )
{
}

