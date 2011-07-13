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

class MCLDContext;
/** \class MCLDFile
 *  \brief A objet file for linking.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCLDFile
{
public:
  MCLDFile();
	~MCLDFile();

	bool isRecognized() const;

private:
	MCLDContext *m_pContext;
	std::string m_Filename;
};

} // namespace of mcld

#endif

