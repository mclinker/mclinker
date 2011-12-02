/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#ifndef MCDATAFRAGMENT_H
#define MCDATAFRAGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCInst.h>
#include <llvm/ADT/SmallString.h>
#include <mcld/LD/Relocation.h>

namespace mcld
{

/** \class MCDataFragment
 *  \brief MCDataFragment for mcld
 *
 *  \see
 *  \author Diana Chen <diana.chen@mediatek.com>
 */
class MCDataFragment : public  llvm::MCFragment
{
private:

  ///  m_pFragment - llvm MCDataFragment for this MCDataFragment
  llvm::MCDataFragment* m_pFragment;
 
  /// m_Relocation - The list of relocations in this fragment
  std::vector<Relocation> m_Relocations;

public:
  typedef std::vector<Relocation>::const_iterator const_relocation_iterator;
  typedef std::vector<Relocation>::iterator relocation_iterator;

public:
  MCDataFragment(llvm::MCDataFragment& pFragment)
    : m_pFragment(&pFragment) {
    setParent( pFragment.getParent() );
    setAtom( pFragment.getAtom() );
    setLayoutOrder( pFragment.getLayoutOrder());
  }
  ~MCDataFragment(){}
   
  // ------ observers ------//
  llvm::SmallString<32> &getContents() { return m_pFragment->getContents();  }
  const llvm::SmallString<32> &getContents() const { return m_pFragment->getContents();  }

  // relocation access
  void addRelocation(Relocation pReloc){  m_Relocations.push_back(pReloc); }
  
  std::vector<Relocation> &getRelocations() { return m_Relocations; }
  const std::vector<Relocation> &getRelcoations() const { return m_Relocations; }

  relocation_iterator relocation_begin() { return m_Relocations.begin(); }
  const_relocation_iterator relocation_begin() const { return m_Relocations.begin(); }

  relocation_iterator relocation_end() {return m_Relocations.end();}
  const_relocation_iterator relocation_end() const {return m_Relocations.end();}

  size_t relocations_size() const { return m_Relocations.size(); }

  // fragment identification
  static bool classof(const MCFragment *pF) {
    return pF->getKind() == llvm::MCFragment::FT_Data;
  }
  static bool classof(const MCDataFragment *) { return true; }
  
  // overwrite parent method
  FragmentType getKind() const { return m_pFragment->getKind(); } 
  
};

} // namespace of mcld

#endif

