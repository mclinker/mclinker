//===- MemoryAreaFactory.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/SystemUtils.h>
#include <mcld/Support/Space.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MemoryAreaFactory
MemoryAreaFactory::MemoryAreaFactory(size_t pNum)
  : GCFactory<MemoryArea, 0>(pNum) {
  // For each loaded file, MCLinker must load ELF header, section header,
  // symbol table, and string table. So, we set the size of chunk quadruple
  // larger than the number of input files.
  m_pRegionFactory = new RegionFactory(pNum*4);
}

MemoryAreaFactory::~MemoryAreaFactory()
{
  HandleToArea::iterator rec, rEnd = m_HandleToArea.end();
  for (rec = m_HandleToArea.begin(); rec != rEnd; ++rec) {
    if (rec->handle->isOpened()) {
      rec->handle->close();
    }
    delete rec->handle;
  }

  delete m_pRegionFactory;
}

MemoryArea*
MemoryAreaFactory::produce(const sys::fs::Path& pPath,
                           FileHandle::OpenMode pMode)
{
  HandleToArea::Result map_result = m_HandleToArea.findFirst(pPath);
  if (NULL == map_result.area) {
    // can not found
    FileHandle* handler = new FileHandle();
    if (!handler->open(pPath, pMode)) {
      error(diag::err_cannot_open_file) << pPath
                                        << sys::strerror(handler->error());
    }

    MemoryArea* result = allocate();
    new (result) MemoryArea(*m_pRegionFactory, *handler);

    m_HandleToArea.push_back(handler, result);
    return result;
  }

  return map_result.area;
}

MemoryArea*
MemoryAreaFactory::produce(const sys::fs::Path& pPath,
                           FileHandle::OpenMode pMode,
                           FileHandle::Permission pPerm)
{
  HandleToArea::Result map_result = m_HandleToArea.findFirst(pPath);
  if (NULL == map_result.area) {
    // can not found
    FileHandle* handler = new FileHandle();
    if (!handler->open(pPath, pMode, pPerm)) {
      error(diag::err_cannot_open_file) << pPath
                                        << sys::strerror(handler->error());
    }

    MemoryArea* result = allocate();
    new (result) MemoryArea(*m_pRegionFactory, *handler);

    m_HandleToArea.push_back(handler, result);
    return result;
  }

  return map_result.area;
}

void MemoryAreaFactory::destruct(MemoryArea* pArea)
{
  m_HandleToArea.erase(pArea);
  pArea->clear();
  pArea->handler()->close();
  destroy(pArea);
  deallocate(pArea);
}

MemoryArea*
MemoryAreaFactory::create(void* pMemBuffer, size_t pSize)
{
  Space* space = new Space(Space::EXTERNAL, pMemBuffer, pSize);
  MemoryArea* result = allocate();
  new (result) MemoryArea(*m_pRegionFactory, *space);
  return result;
}

MemoryArea*
MemoryAreaFactory::create(int pFD, FileHandle::OpenMode pMode)
{
  FileHandle* handler = new FileHandle();
  handler->delegate(pFD, pMode);
  
  MemoryArea* result = allocate();
  new (result) MemoryArea(*m_pRegionFactory, *handler);

  return result;
}

