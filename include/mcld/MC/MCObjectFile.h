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


namespace mcld
{
///forward declaration
class MCObjectFile;
class MemoryBuffer;



class SectionRef
{
private:
	const MCObjectFile *OwningObject;

public:
	SectionRef() : OwningObject(NULL) {}
};


class SymbolRef
{
private:
	const MCObjectFile *OwningObject;

public:
	SymbolRef() : OwningObject(NULL) {}
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
///
///
	

/// sequential access
/// iterator of SymbobRef/SectionRef
///
	template<class content_type>
	class content_iterator 
	{
		content_type Current;
		public:
		content_iterator(content_type symb)
			: Current(symb) {}

		const content_type* operator->() const {
			return &Current;
		}

		bool operator==(const content_iterator &other) const {
			return Current == other.Current;
			/// Section/Symbol implement operator==
		}

		bool operator!=(const content_iterator &other) const {
			return !(*this == other);
			/// Section/Symbol implement operator!=
		}	

		content_iterator& increment() {
			/// FIXME :How to implement getNext() 			
			/// form different formats(ELF,MachOs).
			/// call the corresponding Reader ?
		}


	};

	typedef content_iterator<SymbolRef> symbol_iterator;
	typedef content_iterator<SectionRef> section_iterator;

	symbol_iterator begin_symbols();
	symbol_iterator end_symbols();

	section_iterator begin_sections();
	section_iterator end_sections();

private:
	const llvm::StringRef mFileName;
	MemoryBuffer *mp_Buffer;
	MCLDContext *mp_Context;	
};// end of MCObjectFile






} // namespace of mcld

#endif

