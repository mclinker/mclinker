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
class Module;
class RegionFragment;
class RelocData;

/// ARMExSectionTuple - Tuple of associated exception handling sections
class ARMExSectionTuple {
 public:
  ARMExSectionTuple()
      : m_pTextSection(NULL),
        m_pExIdxSection(NULL) {
  }

  LDSection* getTextSection() const {
    return m_pTextSection;
  }

  LDSection* getExIdxSection() const {
    return m_pExIdxSection;
  }

  void setTextSection(LDSection* pSection) {
    m_pTextSection = pSection;
  }

  void setExIdxSection(LDSection* pSection) {
    m_pExIdxSection = pSection;
  }

  RegionFragment* getTextFragment() const {
    return m_pTextFragment;
  }

  RegionFragment* getExIdxFragment() const {
    return m_pExIdxFragment;
  }

  void setTextFragment(RegionFragment* pFragment) {
    m_pTextFragment = pFragment;
  }

  void setExIdxFragment(RegionFragment* pFragment) {
    m_pExIdxFragment = pFragment;
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
};

/// ARMInputExMap - ARM exception handling section mapping of a mcld::Input.
class ARMInputExMap {
 public:
  typedef std::map<std::string, std::unique_ptr<ARMExSectionTuple> > NameMap;
  typedef NameMap::iterator iterator;
  typedef NameMap::const_iterator const_iterator;

 public:
  // create - Build the exception handling section mapping of a mcld::Input.
  static std::unique_ptr<ARMInputExMap> create(Input &input);

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

  /// begin - return the iterator to the begin of the map
  iterator       begin()       { return m_NameToExData.begin(); }
  const_iterator begin() const { return m_NameToExData.begin(); }

  /// end - return the iterator to the end of the map
  iterator       end()       { return m_NameToExData.end(); }
  const_iterator end() const { return m_NameToExData.end(); }

  /// erase - remove an entry from the map
  void erase(iterator it) { m_NameToExData.erase(it); }

 private:
  ARMInputExMap() = default;

 private:
  NameMap m_NameToExData;
};

/// ARMExData - ARM exception handling data of a mcld::Module.
class ARMExData {
 private:
  typedef std::map<Input*, std::unique_ptr<ARMInputExMap> > InputMap;

  typedef std::map<const Fragment*, ARMExSectionTuple*> ExIdxMap;

 public:
  // create - Build the exception handling section mapping of a mcld::Module.
  static std::unique_ptr<ARMExData> create(Module &module);

  // addInputMap - register the ARMInputExMap with associated pInput
  void addInputMap(Input* pInput,
                   std::unique_ptr<ARMInputExMap> pExMap);

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
  ARMExData() = default;

 private:
  // Map from Input to ARMInputExMap
  InputMap m_Inputs;

  // Map from .ARM.exidx RegionFragment to ARMExSectionTuple
  ExIdxMap m_ExIdxToTuple;
};

}  // namespace mcld

#endif  // TARGET_ARM_ARMEXCEPTION_H_
