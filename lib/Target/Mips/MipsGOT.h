//===- MipsGOT.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MIPS_GOT_H
#define MCLD_MIPS_GOT_H
#include <map>
#include <vector>

#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>

#include <mcld/ADT/SizeTraits.h>
#include <mcld/Target/GOT.h>

namespace mcld
{
class Input;
class LDSection;
class MemoryRegion;
class OutputRelocSection;

/** \class MipsGOTEntry
 *  \brief GOT Entry with size of 4 bytes
 */
class MipsGOTEntry : public GOT::Entry<4>
{
public:
  MipsGOTEntry(uint64_t pContent, SectionData* pParent);

  /// Offset from _gp_disp.
  SizeTraits<32>::Offset getGPRelOffset() const;
};

/** \class MipsGOT
 *  \brief Mips Global Offset Table.
 */
class MipsGOT : public GOT
{
public:
  MipsGOT(LDSection& pSection);

  /// Address of _gp_disp symbol.
  SizeTraits<32>::Address getGPDispAddress() const;

  uint64_t emit(MemoryRegion& pRegion);

  bool reserveLocalEntry(const Input& pInput, const ResolveInfo& pInfo);
  bool reserveGlobalEntry(const Input& pInput, const ResolveInfo& pInfo);

  size_t getLocalNum() const;   ///< number of local symbols in primary GOT
  size_t getGlobalNum() const;  ///< total number of global symbols

  bool isPrimaryGOTConsumed();

  MipsGOTEntry* consumeLocal();
  MipsGOTEntry* consumeGlobal();

  uint64_t getGPAddr(Input& pInput);

  void recordEntry(const Input* pInput,
                   const ResolveInfo* pInfo,
                   MipsGOTEntry* pEntry);
  MipsGOTEntry* lookupEntry(const Input* pInput,
                            const ResolveInfo* pInfo);

  void setLocal(const ResolveInfo* pInfo) {
    m_GOTTypeMap[pInfo] = false;
  }

  void setGlobal(const ResolveInfo* pInfo) {
    m_GOTTypeMap[pInfo] = true;
  }

  bool isLocal(const ResolveInfo* pInfo) {
    return m_GOTTypeMap[pInfo] == false;
  }

  bool isGlobal(const ResolveInfo* pInfo) {
    return m_GOTTypeMap[pInfo] == true;
  }

  /// hasGOT1 - return if this got section has any GOT1 entry
  bool hasGOT1() const;

  bool hasMultipleGOT() const;

  /// Create GOT entries and reserve dynrel entries. 
  void finalizeScanning(OutputRelocSection& pRelDyn);

private:
  /** \class GOTMultipart
   *  \brief GOTMultipart counts local and global entries in the GOT.
   */
  struct GOTMultipart
  {
    GOTMultipart(size_t local = 0, size_t global = 0);

    typedef llvm::DenseSet<const Input*> InputSetType;

    size_t m_LocalNum;  ///< number of reserved local entries
    size_t m_GlobalNum; ///< number of reserved global entries

    size_t m_ConsumedLocal;       ///< consumed local entries
    size_t m_ConsumedGlobal;      ///< consumed global entries

    MipsGOTEntry* m_pLastLocal;   ///< the last consumed local entry
    MipsGOTEntry* m_pLastGlobal;  ///< the last consumed global entry

    InputSetType m_Inputs;

    bool isConsumed() const;

    void consumeLocal();
    void consumeGlobal();
  };

  typedef std::vector<GOTMultipart> MultipartListType;

  typedef llvm::DenseSet<const ResolveInfo*> SymbolSetType;
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolUniqueMapType;

  MultipartListType m_MultipartList;  ///< list of GOT's descriptors
  const Input* m_pInput;              ///< current input
  SymbolSetType m_MergedGlobalSymbols; ///< merged global symbols from
  SymbolUniqueMapType m_InputGlobalSymbols; ///< input global symbols
  SymbolSetType m_MergedLocalSymbols;
  SymbolSetType m_InputLocalSymbols;

  size_t m_CurrentGOTPart;

  size_t m_TotalLocalNum;
  size_t m_TotalGlobalNum;

  void initGOTList(const Input& pInput);
  void changeInput(const Input& pInput);
  bool isGOTFull() const;
  void split();
  void reserve(size_t pNum);

private:
  typedef llvm::DenseMap<const ResolveInfo*, bool> SymbolTypeMapType;

  SymbolTypeMapType m_GOTTypeMap;

private:
  struct GotEntryKey
  {
    const Input* m_pInput;
    const ResolveInfo* m_pInfo;

    bool operator<(const GotEntryKey& key) const
    {
      if (m_pInput == key.m_pInput)
        return m_pInfo < key.m_pInfo;
      else
        return m_pInput < key.m_pInput;
    }
  };

  typedef std::map<GotEntryKey, MipsGOTEntry*> GotEntryMapType;
  GotEntryMapType m_GotEntriesMap;
};

} // namespace of mcld

#endif

