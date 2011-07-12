/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILE_H
#define MCLDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDContext.h>

namespace mcld
{

/** \class MCLDFile
 *  \brief A objet file for linking.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCLDFile
{
	

public:
	typedef MCLDContext::section_iterator section_iterator;
	typedef MCLDContext::const_section_iterator const_section_iterator;

	typedef MCLDContext::symbol_iterator symbol_iteartor;
	typedef MCLDContext::const_symbol_iterator const_symbol_iteartor;
};

private:
	MCLDContext *mpContext;

} // namespace of mcld

#endif

