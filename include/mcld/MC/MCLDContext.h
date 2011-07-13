/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *  Duo <pinronglu@gmail.com>                                                *
 ****************************************************************************/
#ifndef MCLDCONTEXT_H
#define MCLDCONTEXT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/MC/MCAssembler.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>

namespace llvm
{
class MCSection;
class MCSectionData;
} // namespace of llvm

namespace mcld
{
/**
class LDSectionData :public llvm::ilist_node<LDSectionData>{
	LDSectionData(); // DO NOT IMPLEMENT
public:
	LDSectionData(llvm::MCSection &MS) 
		:m_SD(&MS) {	
	}
	~LDSectionData();

private:
	llvm::MCSectionData m_SD;
**/

class MCLDContext
{
/**
private:
	llvm::iplist<LDSectionData> Sections;
	llvm::iplist<llvm::MCSymbolData> Symbols;


public:
	typedef llvm::iplist<llvm::MCSymbolData> SymbolDataListType;

	typedef SymbolDataListType::const_iterator const_symbol_iterator;
	typedef SymbolDataListType::iterator symbol_iterator;

	//FIXME: implement with prag???
	MCLDContext();

	//FIXME: implement in this or MCLDFile
	//       getOrCreate MCSection , pass to MCSectionData( MCSection );
	llvm::MCSectionData getSectionData(llvm::StringRef);
	
	class section_iterator {
	private:
		section_iterator(); // DO NOT IMPLEMENT

		LDSectionData *Current;

	public:

		llvm::MCSectionData operator=(const section_iterator &it) {
			Current = it.Current;
			return *Current;
		}

		bool operator==(const section_iterator &other) const {
			return Current == other.Current;
			/// SectionRef implement operator==
		}

		bool operator!=(const section_iterator &other) const {
			return !(*this == other);
			/// Section implement operator!=
		}

		llvm::MCSectionData &operator*(const section_iterator &it) {
		};

		/// FIXME: copy data when first derefernece
		const llvm::MCSectionData *operator->() const ;

	};

	friend class section_iterator;
	section_iterator begin_section();
	section_iterator end_section();

// FIXME: implement const_section_iterator

**/
};

} // namespace of mcld

#endif

