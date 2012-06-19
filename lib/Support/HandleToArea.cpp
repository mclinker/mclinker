//===- HandleToArea.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/HandleToArea.h>
#include <mcld/Support/MemoryArea.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// HandleToArea
HandleToArea::HandleToArea()
{
}

HandleToArea::~HandleToArea()
{
}

bool HandleToArea::insert(FileHandle* pHandle, MemoryArea* pArea)
{
}

HandleToArea::Result HandleToArea::find(int pHandler)
{
}

HandleToArea::ConstResult HandleToArea::find(int pHandler) const
{
}

HandleToArea::Result HandleToArea::find(const sys::fs::Path& pPath)
{
}

HandleToArea::ConstResult HandleToArea::find(const sys::fs::Path& pPath) const
{
}

