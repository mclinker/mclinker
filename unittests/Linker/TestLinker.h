//===- TestLinker.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TEST_LINKER_H
#define MCLD_TEST_LINKER_H

#include <gtest.h>

#include <string>
#include <list>

#include <mcld/Module.h>
#include <mcld/Object/ObjectLinker.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/LD/DiagnosticPrinter.h>
#include <mcld/LD/DiagnosticLineInfo.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/Path.h>

namespace mcld {

class FileHandle;
class LinkerConfig;
class TargetLDBackend;
class RegionFactory;
class InputFactory;
class MemoryAreaFactory;
class ContextFactory;
class InputBuilder;

namespace test {

class TestLinker
{
public:
  TestLinker();

  ~TestLinker();

  bool initialize(const std::string &pTriple);

  LinkerConfig* config() {
    assert(NULL != m_pConfig);
    return m_pConfig;
  }

  const LinkerConfig* config() const {
    assert(NULL != m_pConfig);
    return m_pConfig;
  }

  const Module* module() const { return &m_Module; }
  Module*       module()       { return &m_Module; }

  // -----  search directories  ----- //
  void addSearchDir(const std::string &pPath);

  void setSysRoot(const mcld::sys::fs::Path &pPath);

  // -----  input operators  ----- //
  void addObject(const std::string &pPath);

  void addObject(const mcld::sys::fs::Path &pPath)
  { addObject(pPath.native()); }

  void addObject(void* pMemBuffer, size_t pSize);

  void addObject(int pFileHandler);

  void addNameSpec(const std::string &pNameSpec);

  bool setOutput(const std::string &pPath);

  bool setOutput(int pFileHandler);

  bool setOutput(const sys::fs::Path &pPath);

  const MemoryArea* getOutput() const { return m_pOutput; }

  MemoryArea*       getOutput()       { return m_pOutput; }

  /// getObjLinker
  ObjectLinker* getObjLinker() {
    assert(NULL != m_pObjLinker);
    return m_pObjLinker;
  }

  /// getObjLinker
  const ObjectLinker* getObjLinker() const {
    assert(NULL != m_pObjLinker);
    return m_pObjLinker;
  }

  /// getLinker
  FragmentLinker* getLinker() {
    assert(NULL != m_pObjLinker);
    return m_pObjLinker->getLinker();
  }

  /// getLinker
  const FragmentLinker* getLinker() const {
    assert(NULL != m_pObjLinker);
    return m_pObjLinker->getLinker();
  }

private:
  void advanceRoot();

private:
  const mcld::Target* m_pTarget;
  mcld::ObjectLinker *m_pObjLinker;
  mcld::LinkerConfig* m_pConfig;
  mcld::Module m_Module;
  mcld::DiagnosticLineInfo* m_pDiagLineInfo;
  mcld::DiagnosticPrinter* m_pDiagPrinter;
  mcld::TargetLDBackend* m_pBackend;
  mcld::InputTree::iterator m_Root;
  mcld::InputFactory* m_pInputFactory;
  mcld::MemoryAreaFactory* m_pMemAreaFactory;
  mcld::ContextFactory* m_pContextFactory;

  mcld::InputBuilder* m_pBuilder;

  std::list<mcld::FileHandle*> m_FileHandleList;
  std::list<mcld::MemoryArea*> m_MemAreaList;

  mcld::MemoryArea* m_pOutput;
};

} // namespace of test
} // namespace of mcld

#endif

