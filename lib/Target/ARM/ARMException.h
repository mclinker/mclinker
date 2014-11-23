//===- ARMException.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARMEXCEPTION_H_
#define TARGET_ARM_ARMEXCEPTION_H_

#include <llvm/ADT/PointerUnion.h>
#include <llvm/ADT/StringRef.h>

#include <map>
#include <memory>
#include <string>

namespace mcld {

class Fragment;
class Input;
class LDSection;
class RegionFragment;
class RelocData;

/// ARMExSectionTuple - Tuple of associated exception handling sections
class ARMExSectionTuple {
 public:
  ARMExSectionTuple()
      : m_pTextSection(NULL),
        m_pExIdxSection(NULL),
        m_pExTabSection(NULL),
        m_pRelExIdxSection(NULL),
        m_pRelExTabSection(NULL) {
  }

  LDSection* getTextSection() const {
    return m_pTextSection;
  }

  LDSection* getExIdxSection() const {
    return m_pExIdxSection;
  }

  LDSection* getExTabSection() const {
    return m_pExTabSection;
  }

  LDSection* getRelExIdxSection() const {
    return m_pRelExIdxSection;
  }

  LDSection* getRelExTabSection() const {
    return m_pRelExTabSection;
  }

  void setTextSection(LDSection* pSection) {
    m_pTextSection = pSection;
  }

  void setExIdxSection(LDSection* pSection) {
    m_pExIdxSection = pSection;
  }

  void setExTabSection(LDSection* pSection) {
    m_pExTabSection = pSection;
  }

  void setRelExIdxSection(LDSection* pSection) {
    m_pRelExIdxSection = pSection;
  }

  void setRelExTabSection(LDSection* pSection) {
    m_pRelExTabSection = pSection;
  }

  RegionFragment* getTextFragment() const {
    return m_pTextFragment;
  }

  RegionFragment* getExIdxFragment() const {
    return m_pExIdxFragment;
  }

  RegionFragment* getExTabFragment() const {
    return m_pExTabFragment;
  }

  RelocData* getExIdxRelocData() const {
    return m_pExIdxRelocData;
  }

  RelocData* getExTabRelocData() const {
    return m_pExTabRelocData;
  }

  void setTextFragment(RegionFragment* pFragment) {
    m_pTextFragment = pFragment;
  }

  void setExIdxFragment(RegionFragment* pFragment) {
    m_pExIdxFragment = pFragment;
  }

  void setExTabFragment(RegionFragment* pFragment) {
    m_pExTabFragment = pFragment;
  }

  void setExIdxRelocData(RelocData* pRelocData) {
    m_pExIdxRelocData = pRelocData;
  }

  void setExTabRelocData(RelocData* pRelocData) {
    m_pExTabRelocData = pRelocData;
  }

 private:
  // .text section
  union {
    LDSection*      m_pTextSection;
    RegionFragment* m_pTextFragment;
  };

  // .ARM.exidx section
  union {
    LDSection*      m_pExIdxSection;
    RegionFragment* m_pExIdxFragment;
  };

  // .ARM.extab section
  union {
    LDSection*      m_pExTabSection;
    RegionFragment* m_pExTabFragment;
  };

  // .rel.ARM.exidx section
  union {
    LDSection*      m_pRelExIdxSection;
    RelocData*      m_pExIdxRelocData;
  };

  // .rel.ARM.extab section
  union {
    LDSection*      m_pRelExTabSection;
    RelocData*      m_pExTabRelocData;
  };
};

/// ARMInputExMap - ARM exception handling data of an Input
class ARMInputExMap {
 public:
  typedef std::map<std::string, std::unique_ptr<ARMExSectionTuple> > NameMap;
  typedef NameMap::iterator iterator;
  typedef NameMap::const_iterator const_iterator;

 public:
  ARMInputExMap() { }

  /// get - Get the ARMExSectionTuple by the corresponding text section name.
  /// As an exception, to get the ARMExSectionTuple for .text section, use ""
  /// as the section name instead.
  ARMExSectionTuple* get(const char* pName) const {
    NameMap::const_iterator it = m_NameToExData.find(pName);
    if (it == m_NameToExData.end()) {
      return NULL;
    }
    return it->second.get();
  }

  ARMExSectionTuple* getByExSection(llvm::StringRef pName) const {
    assert((pName.startswith(".ARM.exidx") ||
            pName.startswith(".ARM.extab")) &&
           "Not a .ARM.exidx section name");
    return get(pName.data() + sizeof(".ARM.ex***") - 1);
  }

  ARMExSectionTuple* getByRelExSection(llvm::StringRef pName) const {
    assert((pName.startswith(".rel.ARM.exidx") ||
            pName.startswith(".rel.ARM.extab")) &&
           "Not a .rel.ARM.exidx section name");
    return get(pName.data() + sizeof(".rel.ARM.ex***") - 1);
  }

  /// getOrCreate - Get an existing or create a new ARMExSectionTuple which is
  /// associated with the text section name.  As an exception, use "" as the
  /// section name for .text section.
  ARMExSectionTuple* getOrCreate(const char* pName) {
    std::unique_ptr<ARMExSectionTuple>& result = m_NameToExData[pName];
    if (!result) {
      result.reset(new ARMExSectionTuple());
    }
    return result.get();
  }

  ARMExSectionTuple* getOrCreateByExSection(llvm::StringRef pName) {
    assert((pName.startswith(".ARM.exidx") ||
            pName.startswith(".ARM.extab")) &&
           "Not a .ARM.exidx section name");
    return getOrCreate(pName.data() + sizeof(".ARM.ex***") - 1);
  }

  ARMExSectionTuple* getOrCreateByRelExSection(llvm::StringRef pName) {
    assert((pName.startswith(".rel.ARM.exidx") ||
            pName.startswith(".rel.ARM.extab")) &&
           "Not a .rel.ARM.exidx section name");
    return getOrCreate(pName.data() + sizeof(".rel.ARM.ex***") - 1);
  }

  /// begin - return the iterator to the begin of the map
  iterator       begin()       { return m_NameToExData.begin(); }
  const_iterator begin() const { return m_NameToExData.begin(); }

  /// end - return the iterator to the end of the map
  iterator       end()       { return m_NameToExData.end(); }
  const_iterator end() const { return m_NameToExData.end(); }

  /// erase - remove an entry from the map
  void erase(iterator it) { m_NameToExData.erase(it); }

 private:
  NameMap m_NameToExData;
};

/// ARMExData - ARM exception handling data of a module
class ARMExData {
 private:
  typedef std::map<Input*, std::unique_ptr<ARMInputExMap> > InputMap;

  typedef std::map<const Fragment*, ARMExSectionTuple*> ExIdxMap;

 public:
  ARMExData() { }

  // addInputMap - register the ARMInputExMap with associated pInput
  void addInputMap(Input* pInput,
                   std::unique_ptr<ARMInputExMap>&& pExMap);

  // getInputMap - get the ARMInputExMap corresponding to pInput
  ARMInputExMap* getInputMap(Input* pInput) const {
    InputMap::const_iterator it = m_Inputs.find(pInput);
    if (it == m_Inputs.end()) {
      return NULL;
    }
    return it->second.get();
  }

  // getTupleByExIdx - get the ARMExSectionTuple corresponding to pExIdxFragment
  ARMExSectionTuple* getTupleByExIdx(const Fragment* pExIdxFragment) const {
    ExIdxMap::const_iterator it = m_ExIdxToTuple.find(pExIdxFragment);
    if (it == m_ExIdxToTuple.end()) {
      return NULL;
    }
    return it->second;
  }

 private:
  // Map from Input to ARMInputExMap
  InputMap m_Inputs;

  // Map from .ARM.exidx RegionFragment to ARMExSectionTuple
  ExIdxMap m_ExIdxToTuple;
};

}  // namespace mcld

#endif  // TARGET_ARM_ARMEXCEPTION_H_
