//===- Module.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Module contains the intermediate representation (LDIR) of MCLinker.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MODULE_H
#define MCLD_MODULE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <string>

#include <llvm/ADT/ilist.h>

#include <mcld/Support/GCFactoryListTraits.h>
#include <mcld/LD/Fragment.h>
#include <mcld/LD/Relocation.h>
#include <mcld/LD/NamePool.h>
#include <mcld/MC/SymbolCategory.h>
#include <mcld/MC/MCLDInput.h>

namespace mcld {

/** \class Module
 *  \brief Module provides the intermediate representation for linking.
 */
class Module
{
public:
  typedef std::vector<Input*> ObjectList;
  typedef ObjectList::iterator obj_iterator;
  typedef ObjectList::const_iterator const_obj_iterator;

  typedef std::vector<Input*> LibraryList;
  typedef LibraryList::iterator lib_iterator;
  typedef LibraryList::const_iterator const_lib_iterator;

  typedef std::vector<LDSection*> SectionTable;
  typedef SectionTable::iterator iterator;
  typedef SectionTable::const_iterator const_iterator;

  typedef SymbolCategory SymbolTable;
  typedef SymbolTable::iterator sym_iterator;
  typedef SymbolTable::const_iterator const_sym_iterator;

  typedef llvm::iplist<Fragment,
                       GCFactoryListTraits<Fragment> > RelocationTable;
  typedef RelocationTable::iterator reloc_iterator;
  typedef RelocationTable::const_iterator const_reloc_iterator; 

public:
  Module();

  Module(const std::string& pName);

  ~Module();

  // -----  name  ----- //
  const std::string& name() const { return m_Name; }

  void setName(const std::string& pName) { m_Name = pName; }

  // -----  link-in libraries  ----- //
  const LibraryList& getLibraryList() const { return m_LibraryList; }
  LibraryList&       getLibraryList()       { return m_LibraryList; }

  const_lib_iterator lib_begin() const { return m_LibraryList.begin(); }
  lib_iterator       lib_begin()       { return m_LibraryList.begin(); }
  const_lib_iterator lib_end  () const { return m_LibraryList.end();   }
  lib_iterator       lib_end  ()       { return m_LibraryList.end();   }

/// @}
/// @name Section Accessors
/// @{

  // -----  sections  ----- //
  const SectionTable& getSectionTable() const { return m_SectionTable; }
  SectionTable&       getSectionTable()       { return m_SectionTable; }

  iterator         begin()       { return m_SectionTable.begin(); }
  const_iterator   begin() const { return m_SectionTable.begin(); }
  iterator         end  ()       { return m_SectionTable.end();   }
  const_iterator   end  () const { return m_SectionTable.end();   }
  LDSection*       front()       { return m_SectionTable.front(); }
  const LDSection* front() const { return m_SectionTable.front(); }
  LDSection*       back ()       { return m_SectionTable.back();  }
  const LDSection* back () const { return m_SectionTable.back();  }
  size_t           size () const { return m_SectionTable.size();  }
  bool             empty() const { return m_SectionTable.empty(); }

  // Following two functions will be obsolette when we have new section merger.
  LDSection*       getSection(const std::string& pName);
  const LDSection* getSection(const std::string& pName) const;

/// @}
/// @name Symbol Accessors
/// @{

  // -----  symbols  ----- //
  const SymbolTable& getSymbolTable() const { return m_SymbolTable; }
  SymbolTable&       getSymbolTable()       { return m_SymbolTable; }

  sym_iterator       sym_begin()       { return m_SymbolTable.begin();         }
  const_sym_iterator sym_begin() const { return m_SymbolTable.begin();         }
  sym_iterator       sym_end  ()       { return m_SymbolTable.end();           }
  const_sym_iterator sym_end  () const { return m_SymbolTable.end();           }
  size_t             sym_size () const { return m_SymbolTable.numOfSymbols();  }

  // -----  names  ----- //
  const NamePool& getNamePool() const { return m_NamePool; }
  NamePool&       getNamePool()       { return m_NamePool; }

/// @}
/// @name Relocation Accessors
/// @{

  // -----  relocations  ----- //
  RelocationTable&       getRelocationTable()       { return m_RelocTable; }
  const RelocationTable& getRelocationTable() const { return m_RelocTable; }

  reloc_iterator       reloc_begin()       { return m_RelocTable.begin(); }
  const_reloc_iterator reloc_begin() const { return m_RelocTable.begin(); }
  reloc_iterator       reloc_end  ()       { return m_RelocTable.end();   }
  const_reloc_iterator reloc_end  () const { return m_RelocTable.end();   }
  
private:
  std::string m_Name;
  ObjectList m_ObjectList;
  LibraryList m_LibraryList;
  SectionTable m_SectionTable;
  SymbolTable m_SymbolTable;
  NamePool m_NamePool;
  RelocationTable m_RelocTable;
};

} // namespace of mcld

#endif

