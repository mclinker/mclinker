//===- AndroidSectLinker.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/AndroidSectLinker.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDDirectory.h>

#include <llvm/Module.h>

#include <sstream>

using namespace mcld;

//==========================
// AndroidSectLinker

AndroidSectLinker::AndroidSectLinker(const std::string &pInputFilename,
                                     const std::string &pOutputFilename,
                                     unsigned int OutputLinkType,
                                     MCLDInfo &pLDInfo,
                                     TargetLDBackend &pLDBackend)
  : SectLinker(pInputFilename,
               pOutputFilename,
               OutputLinkType,
               pLDInfo,
               pLDBackend) {
}

AndroidSectLinker::~AndroidSectLinker()
{
  // SectLinker will delete m_pLDBackend and m_pLDDriver;
}

bool AndroidSectLinker::doInitialization(Module &pM)
{
  // -----  Set up General Options  ----- //
  //   make sure output is openend successfully.
  if (!m_LDInfo.output().hasMemArea())
    report_fatal_error("output is not given on the command line\n");

  if (!m_LDInfo.output().memArea()->isGood())
    report_fatal_error(
      "can not open output file :" + m_LDInfo.output().path().native());

  InputTree::iterator iter = m_LDInfo.inputs().begin();

  for (Module::LibraryListType::const_iterator I = pM.lib_begin(),
    E = pM.lib_end(); I != E; ++I) {
    std::ostringstream libName;
    std::ostringstream libPath;
    libName << "l" << *I;
    // For now, libPath must in /system/lib
    libPath << "/system/lib/lib" << *I << ".so";
    Input* deplib = m_LDInfo.inputFactory().produce(
                      libName.str(), sys::fs::Path(libPath.str()));
    m_LDInfo.inputs().insert<InputTree::Positional> (iter, *deplib);
    iter++;
  }

  // Now, all input arguments are prepared well, send it into MCLDDriver
  m_pLDDriver = new MCLDDriver(m_LDInfo, *m_pLDBackend);
  return false;
}

