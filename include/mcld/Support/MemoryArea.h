/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_MEMORY_AREA_H
#define MCLD_MEMORY_AREA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>
#include <string>

namespace mcld
{

class MemoryRegion;

/** \class MemoryArea
 *  \brief MemoryArea is used to manage distinct MemoryRegions of address space.
 *
 *  Entire allocated memory space in MemoryArea is fixed.
 *  MemoryArea is a continue address space.
 *  MemoryArea is allowed to increase it size. If its size can not be increased,
 *  a fatal error is raised.
 *
 */
class MemoryArea : private Uncopyable
{
public:
  typedef unsigned char* Address;

private:
  struct Space
  {
  public:
    enum Ownership
    {
      ALLOCATED_ARRAY,
      MMAPED,
      NOT_OWNED
    };

  public:
    Space()
    : next(0), ownership(NOT_OWNED), data(0), start(0), size(0)
    { }

    ~Space();

  public:  
    Space* next;
    Ownership ownership;
    const Address data;
    Address start;
    size_t size;
  };

public:
  MemoryArea();
  ~MemoryArea();

  // claim - create a MemoryRegion within a sufficient space
  MemoryRegion* claim(Address pStart, size_t pLength);

  void open(const std::string& pName, int flags, int mode);
  void close();
  bool isGood() const;

private:
  // find - first fit search
  Space* find(Address pStart, size_t pLength);

private:
  intptr_t m_FileDescriptor;
  Space* m_Root;

};

} // namespace of mcld

#endif

