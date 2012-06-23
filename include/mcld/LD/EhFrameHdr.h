//===- EhFrameHdr.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_EHFRAMEHDR_H
#define MCLD_EHFRAMEHDR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/Dwarf.h>
#include <llvm/Support/DataTypes.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/CIE.h>
#include <mcld/LD/FDE.h>
#include <mcld/LD/Layout.h>

namespace mcld
{
class EhFrame;
class LDSection;
class Output;
class FDE;
class MCLinker;

/** \class EhFrameHdr
 *  \brief EhFrameHdr represents .eh_frame_hdr section.
 */
class EhFrameHdr
{
public:
  EhFrameHdr(const EhFrame& pEhFrameData,
             const LDSection& pEhFrameSect,
             LDSection& pEhFrameHdrSect);

  ~EhFrameHdr();

  /// sizeOutput - base on the fde count to size output
  void sizeOutput();

  /// emitOutput - write out eh_frame_hdr
  template<size_t size>
  void emitOutput(Output& pOutput, MCLinker& pLinker);

private:
  /// getFDEPC - return the address of FDE's pc
  /// @param pFDE - FDE
  /// @param pOffset - the output offset of FDE
  template<size_t size>
  typename SizeTraits<size>::Address
  getFDEPC(const FDE& pFDE,
           typename SizeTraits<size>::Offset pOffset,
           const MemoryRegion& pEhFrameRegion);

  template<size_t size>
  class BSTEntry
  {
  public:
    typedef std::pair<typename SizeTraits<size>::Address,
                      typename SizeTraits<size>::Address> EntryType;
  };

  template<size_t size>
  struct BSTEntryCompare
    : public std::binary_function<const typename BSTEntry<size>::EntryType&,
                                  const typename BSTEntry<size>::EntryType&,
                                  bool>
  {
    bool operator()(const typename BSTEntry<size>::EntryType& X,
                    const typename BSTEntry<size>::EntryType& Y) const
    { return X.first < Y.first; }
  };

private:
  /// eh_frame data
  const EhFrame& m_EhFrameData;

  /// .eh_frame section
  const LDSection& m_EhFrameSect;

  /// .eh_frame_hdr section
  LDSection& m_EhFrameHdrSect;
};

#include "EhFrameHdr.tcc"

} // namespace of mcld

#endif

