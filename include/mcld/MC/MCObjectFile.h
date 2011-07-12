/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCOBJECTFILE_H
#define MCOBJECTFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/MC/MCAssembler.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/MemoryBuffer.h"


namespace mcld
{
///  Proxy of llvm::MCSectionData
class MCSectionData :public llvm::ilist_node<MCSectionData> {
	friend class MCObjectFile;
public:
	MCSectionData() {
	};
	~MCSectionData(){
	};
	
	MCSectionData &operator= ( MCSectionData & );
	///DO NOT do memory copy , just create MCSectionData 	

private:
	llvm::MCSectionData *mpSectionData;
};




/** \class MCObjectFile
 *  \brief MCObjectFile is the data structure for linking.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCObjectFile {
private:
	MCObjectFile(); // Do not implement

public:
	MCObjectFile(llvm::StringRef FilePath);
	~MCObjectFile();

	MCSectionData getSectionData(llvm::StringRef SectionName);

	void Test_iterator(){
		llvm::ilist_iterator<MCSectionData> it = Sections.begin();
		MCSectionData *p = new MCSectionData();
		Sections.insert(it,p);
		it++;
	}


private:
	llvm::StringRef mFileName;

	llvm::OwningPtr<llvm::MemoryBuffer> mpBuffer;

	llvm::iplist<MCSectionData> Sections;
	llvm::iplist<llvm::MCSymbolData> Symbols;


	/// FIXME
	//llvm::DenseMap<llvm::MCSection, MCSectionData*> SectionMap;
	//llvm::DenseMap<llvm::MCSymbol, llvm::MCSymbolData*> SymbolMap;


///
/// iterator of SectionRef
///

public: 
	class section_iterator 
	{
		private:
		section_iterator(); // DO NOT IMPLEMENT
		
		MCSectionData *Current;
	
		public:
		
		MCSectionData operator=(const section_iterator &it) {
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
		
		MCSectionData &operator*();
		/// FIXME: copy data when first derefernece

		const MCSectionData *operator->() const ;


	};

	friend class section_iterator;
	section_iterator begin_section();
	section_iterator end_section();

/// End of MCObjectFile::section_iterator


// MCObjectFile::symbol_iterator

	class symbol_iterator 
	{
		symbol_iterator(); // DO NOT IMPLEMENT
	
		llvm::MCSymbolData *Current;

		public:
		
		llvm::MCSymbolData operator=(const symbol_iterator &it) {
			Current = it.Current;	
			return *Current;
		}

		bool operator==(const symbol_iterator &other) const {
			return Current == other.Current;
			/// Section implement operator==
		}

		bool operator!=(const symbol_iterator &other) const {
			return !(*this == other);
			/// Section implement operator!=
		}	

		llvm::MCSymbolData &operator*();
		/// FIXME: copy data when first dereference

		const llvm::MCSymbolData *operator->() const;
	
	};

	friend class symbol_iterator;
	symbol_iterator begin_symbol() {
					
	};

	symbol_iterator end_symbol() {
	
	};

/// End of MCObjectFile::symbol_iterator

};// end of MCObjectFile


} // namespace of mcld

#endif

