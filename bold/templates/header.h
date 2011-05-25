/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   ${AUTHOR} <${EMAIL}>                                                    *
 ****************************************************************************/
#ifndef ${CLASS_NAME}_H
#define ${CLASS_NAME}_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace BOLD
{

/** \class ${class_name}
 *  \brief ${brief}
 *
 *  \see
 *  \author ${AUTHOR} <${EMAIL}>
 */
class ${class_name}
{

#ifdef ENABLE_UNITTEST
FRIEND_TEST( ${class_name}Test, method_name);
#endif
};

} // namespace of BOLD

#endif

