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
///forward declaration
class MCObjectFile;

/// SectionRef means Section header in linkint layer
class SectionRef
{
private:
	const MCObjectFile *OwningObject;
	const llvm::MCSectionData *OwningSectionData;
public:	
	SectionRef() : OwningObject(NULL) {}

	bool operator==(const SectionRef& ) const;
};


class SymbolRef
{
private:
	const MCObjectFile *OwningObject;
	const llvm::MCSymbolData *OwningSymbolData;
public:
	SymbolRef() : OwningObject(NULL) {}

	bool operator==(const SymbolRef& ) const;
};


class MCLDContext
{	
  llvm::iplist<llvm::MCSectionData> Sections;
	llvm::iplist<llvm::MCSymbolData> Symbols;

/// MCSection header
/// FIXME
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

/// random access
	
/// sequential access
/// iterator of SectionRef
	
	class section_iterator 
	{
		SectionRef Current;
		public:
		section_iterator(SectionRef sectb)
			: Current(sectb) {}

		const SectionRef* operator->() const {
			return &Current;
		}

		bool operator==(const section_iterator &other) const {
			return Current == other.Current;
			/// SectionRef implement operator==
		}

		bool operator!=(const section_iterator &other) const {
			return !(*this == other);
			/// Section implement operator!=
		}	

		section_iterator& increment() {
			/// FIXME :How to implement getNext() 			
			/// form different formats(ELF,MachOs).
			/// call the corresponding Reader ?
		}

		
		llvm::MCSectionData &operator*();
		/// FIXME: copy data when first derefernece

	};

	section_iterator begin_sections();
	section_iterator end_sections();

/// End of MCObjectFile::section_iterator


// MCObjectFile::symbol_iterator

	class symbol_iterator 
	{
		SymbolRef Current;
		public:
		symbol_iterator(SymbolRef symb)
			: Current(symb) {}

		const SymbolRef* operator->() const {
			return &Current;
		}

		bool operator==(const symbol_iterator &other) const {
			return Current == other.Current;
			/// Section implement operator==
		}

		bool operator!=(const symbol_iterator &other) const {
			return !(*this == other);
			/// Section implement operator!=
		}	

		symbol_iterator& increment() {
			/// FIXME :How to implement getNext() 			
			/// form different formats(ELF,MachOs).
			/// call the corresponding Reader ?
		}

		llvm::MCSymbolData &operator*();
		/// FIXME: copy data when first dereference

	};


	symbol_iterator begin_symbols();
	symbol_iterator end_symbols();

/// End of MCObjectFile::symbol_iterator



private:
	const llvm::StringRef mFileName;

	const llvm::OwningPtr<llvm::MemoryBuffer> mpBuffer;

	MCLDContext *mpContext;	

};// end of MCObjectFile


} // namespace of mcld

#endif

