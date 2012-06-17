//===- FileHandle.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/FileHandle.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// FileHandle
FileHandle::FileHandle()
{
}


FileHandle::~FileHandle()
{
}


bool FileHandle::open(const sys::fs::Path& pPath,
                      enum FileHandle::OpenMode pMode)
{
}


bool FileHandle::open(const sys::fs::Path& pPath,
                      enum FileHandle::OpenMode pMode,
                      enum FileHandle::Permission pPerm)
{
}


bool FileHandle::delegate(int& pFD, enum FileHandle::OpenMode pMode)
{
}


void FileHandle::close()
{
}


void FileHandle::setState(FileHandle::IOState pState)
{
}


void FileHandle::clear(FileHandle::IOState pState)
{
}


const sys::fs::Path& FileHandle::path() const
{
}


size_t FileHandle::size() const
{
}


bool FileHandle::isOpened() const
{
}


bool FileHandle::isGood() const
{
}


bool FileHandle::isBad() const
{
}


bool FileHandle::isFailed() const
{
}


bool FileHandle::isReadable() const
{
}


bool FileHandle::isWritable() const
{
}


bool FileHandle::isReadWrite() const
{
}


int FileHandle::rdstate() const
{
}

