/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/CodeGen/MCLDPrinter.h>
#include <llvm/Module.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDScriptReader.h>

using namespace mcld;
using namespace llvm;

//==========================
// MCLDPrinter
LDPrinter::LDPrinter(MCLDBackend &pBackend, MCLinker &pLinker)
: m_pLDInfo( new MCLDInfo() ), m_Linker(pLinker)
{
  m_pScriptReader = new MCLDScriptReader( *m_pLDInfo );
}

LDPrinter::~LDPrinter()
{
  delete m_pLDInfo;
  delete m_pScriptReader;
}

bool LDPrinter::doInitialization(Module &pM)
{
  m_Linker.setLayout( m_pScriptReader->createLayout() )

  while( m_pScriptReader->hasCommand() ) {
    m_Linker.addCommand( m_pScriptReader->createCommmand() );
  }
}

bool LDPrinter::doFinalization(Module &pM)
{
}

bool LDPrinter::runOnMachineFunction( MachineFunction& pF )
{
  m_Linker.run();
}


