//===- TestLinker.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "TestLinker.h"

#include <llvm/Support/TargetSelect.h>

#include <mcld/LD/TextDiagnosticPrinter.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Support/Space.h>
#include <mcld/Support/TargetSelect.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/SystemUtils.h>
#include <mcld/Support/MemoryAreaFactory.h>

using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcld::test;

//===----------------------------------------------------------------------===//
// TestLinker
//===----------------------------------------------------------------------===//
TestLinker::TestLinker()
  : m_pTarget(NULL), m_pObjLinker(NULL), m_pConfig(NULL), m_pDiagLineInfo(NULL),
    m_pDiagPrinter(NULL), m_pBackend(NULL),
    m_pMemAreaFactory(NULL), m_pOutput(NULL) {
}

TestLinker::~TestLinker()
{
  std::list<mcld::FileHandle*>::iterator file, fEnd = m_FileHandleList.end();
  for (file = m_FileHandleList.begin(); file != fEnd; ++file)
    delete (*file);

  std::list<mcld::MemoryArea*>::iterator mem, mEnd = m_MemAreaList.end() ;
  for (mem = m_MemAreaList.begin(); mem != mEnd; ++mem)
    delete (*mem);

  delete m_pObjLinker;
  delete m_pConfig;
  delete m_pDiagLineInfo;
  delete m_pDiagPrinter;
  delete m_pBackend;
  delete m_pMemAreaFactory;
  delete m_pOutput;
}

bool TestLinker::initialize(const std::string &pTriple)
{
  bool is_initialized = false;

  if (is_initialized)
    return false;

  // initilaize all llvm::Target and mcld::Target
  llvm::InitializeAllTargets();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllTargetMCs();
  mcld::InitializeAllTargets();
  mcld::InitializeAllDiagnostics();

  // create mcld::LinkerConfig
  m_pConfig = new LinkerConfig(pTriple, 1, 32);
  m_Root = m_pConfig->inputs().root();

  // specify mcld::Target
  std::string error;
  m_pTarget = mcld::TargetRegistry::lookupTarget(pTriple, error);
  if (NULL == m_pTarget) {
    fatal(diag::fatal_cannot_init_target) << pTriple << error;
    return false;
  }

  // create mcld::DiagnosticEngine
  m_pDiagLineInfo = m_pTarget->createDiagnosticLineInfo(*m_pTarget, pTriple);
  if (NULL == m_pDiagLineInfo) {
    fatal(diag::fatal_cannot_init_lineinfo) << pTriple;
    return false;
  }

  m_pDiagPrinter = new mcld::TextDiagnosticPrinter(mcld::errs(), *m_pConfig);

  mcld::InitializeDiagnosticEngine(*m_pConfig, m_pDiagLineInfo, m_pDiagPrinter);

  // create mcld::TargetLDBackend
  m_pBackend = m_pTarget->createLDBackend(pTriple);
  if (NULL == m_pBackend) {
    fatal(diag::fatal_cannot_init_backend) << pTriple;
    return false;
  }

  m_pMemAreaFactory = new MemoryAreaFactory(32);

  m_pObjLinker = new mcld::ObjectLinker(*m_pConfig, *m_pBackend, m_Module, *m_pMemAreaFactory);
  m_pObjLinker->initFragmentLinker();

  is_initialized = true;
  return true;
}

void TestLinker::addSearchDir(const std::string &pDirPath)
{
  assert(NULL != m_pConfig && "initialize() must be called before addSearchDir");
  assert(!m_pConfig->options().sysroot().empty() &&
         "must setSysRoot before addSearchDir");

  mcld::MCLDDirectory* sd = new mcld::MCLDDirectory(pDirPath);

  if (sd->isInSysroot()) {
    sd->setSysroot(m_pConfig->options().sysroot());
  }

  if (exists(sd->path()) && is_directory(sd->path())) {
    m_pConfig->options().directories().add(*sd);
  } else {
    mcld::warning(mcld::diag::warn_cannot_open_search_dir) << sd->name();
  }
}

void TestLinker::setSysRoot(const mcld::sys::fs::Path &pPath)
{
  assert(NULL != m_pConfig && "initialize() must be called before setSysRoot");
  m_pConfig->options().setSysroot(pPath);
}

void TestLinker::addObject(const std::string &pPath)
{
  mcld::Input* input = m_pConfig->inputFactory().produce(pPath, pPath,
                                                       mcld::Input::Unknown);

  m_pConfig->inputs().insert<mcld::InputTree::Positional>(m_Root, *input);

  advanceRoot();

  mcld::FileHandle* handler = new mcld::FileHandle();
  m_FileHandleList.push_back(handler);
  if (!handler->open(pPath, mcld::FileHandle::ReadOnly)) {
    mcld::error(mcld::diag::err_cannot_open_file)
                                      << pPath
                                      << mcld::sys::strerror(handler->error());
  }

  mcld::MemoryArea* input_memory = new MemoryArea(*handler);
  input->setMemArea(input_memory);
  m_MemAreaList.push_back(input_memory);

  mcld::LDContext* context = m_pConfig->contextFactory().produce(pPath);
  input->setContext(context);
}

void TestLinker::addObject(void* pMemBuffer, size_t pSize)
{
  mcld::Input* input = m_pConfig->inputFactory().produce("memory object", "NAN",
                                                       mcld::Input::Unknown);

  m_pConfig->inputs().insert<mcld::InputTree::Positional>(m_Root, *input);

  advanceRoot();

  mcld::Space* space = new mcld::Space(mcld::Space::EXTERNAL, pMemBuffer, pSize);
  mcld::MemoryArea* input_memory = new MemoryArea(*space);
  input->setMemArea(input_memory);
  m_MemAreaList.push_back(input_memory);

  mcld::LDContext* context = m_pConfig->contextFactory().produce();
  input->setContext(context);
}

void TestLinker::addObject(int pFileHandler)
{
  mcld::Input* input = m_pConfig->inputFactory().produce("handler object", "NAN",
                                                       mcld::Input::Unknown);

  m_pConfig->inputs().insert<mcld::InputTree::Positional>(m_Root, *input);

  advanceRoot();

  mcld::FileHandle* handler = new mcld::FileHandle();
  m_FileHandleList.push_back(handler);
  handler->delegate(pFileHandler);

  mcld::MemoryArea* input_memory = new MemoryArea(*handler);
  input->setMemArea(input_memory);
  m_MemAreaList.push_back(input_memory);

  mcld::LDContext* context = m_pConfig->contextFactory().produce();
  input->setContext(context);
}

void TestLinker::addNameSpec(const std::string &pNameSpec)
{
  mcld::sys::fs::Path* path = NULL;
  // find out the real path of the namespec.
  if (m_pConfig->attrFactory().constraint().isSharedSystem()) {
    // In the system with shared object support, we can find both archive
    // and shared object.

    if (m_pConfig->attrFactory().last().isStatic()) {
      // with --static, we must search an archive.
      path = m_pConfig->options().directories().find(pNameSpec,
                                                   mcld::Input::Archive);
    }
    else {
      // otherwise, with --Bdynamic, we can find either an archive or a
      // shared object.
      path = m_pConfig->options().directories().find(pNameSpec,
                                                   mcld::Input::DynObj);
    }
  }
  else {
    // In the system without shared object support, we only look for an
    // archive.
    path = m_pConfig->options().directories().find(pNameSpec,
                                                 mcld::Input::Archive);
  }

  if (NULL == path) {
    mcld::fatal(diag::err_cannot_find_namespec) << pNameSpec;
    return;
  }

  mcld::Input* input = m_pConfig->inputFactory().produce(pNameSpec, *path,
                                                         mcld::Input::Unknown);

  m_pConfig->inputs().insert<mcld::InputTree::Positional>(m_Root, *input);

  advanceRoot();

  mcld::FileHandle* handler = new mcld::FileHandle();
  m_FileHandleList.push_back(handler);
  if (!handler->open(*path, mcld::FileHandle::ReadOnly)) {
    mcld::error(mcld::diag::err_cannot_open_file)
                                      << *path
                                      << mcld::sys::strerror(handler->error());
  }

  mcld::MemoryArea* input_memory = new MemoryArea(*handler);
  input->setMemArea(input_memory);
  m_MemAreaList.push_back(input_memory);

  mcld::LDContext* context = m_pConfig->contextFactory().produce(*path);
  input->setContext(context);
}

bool TestLinker::setOutput(const std::string &pPath)
{
  if (m_pConfig->output().hasContext())
    return false;

  mcld::FileHandle* handler = new mcld::FileHandle();
  m_FileHandleList.push_back(handler);
  bool open_res = handler->open(pPath, mcld::FileHandle::ReadWrite |
                                       mcld::FileHandle::Truncate |
                                       mcld::FileHandle::Create,
                                mcld::FileHandle::Permission(0755));
  if (!open_res) {
    mcld::error(mcld::diag::err_cannot_open_file)
                                      << pPath
                                      << mcld::sys::strerror(handler->error());
  }

  m_pOutput = new MemoryArea(*handler);

  mcld::LDContext* context = m_pConfig->contextFactory().produce(pPath);
  m_pConfig->output().setContext(context);

  // FIXME: remove the initStdSections().
  m_pObjLinker->initStdSections();
  return true;
}

bool TestLinker::setOutput(const sys::fs::Path &pPath)
{
  return setOutput(pPath.native());
}

bool TestLinker::setOutput(int pFileHandler)
{
  if (m_pConfig->output().hasContext())
    return false;

  mcld::FileHandle* handler = new mcld::FileHandle();
  handler->delegate(pFileHandler);
  m_FileHandleList.push_back(handler);

  m_pOutput = new MemoryArea(*handler);

  mcld::LDContext* context = m_pConfig->contextFactory().produce();
  m_pConfig->output().setContext(context);

  // FIXME: remove the initStdSections().
  m_pObjLinker->initStdSections();
  return true;
}

void TestLinker::advanceRoot()
{
    if (m_Root.isRoot())
      --m_Root;
    else
      ++m_Root;
}
