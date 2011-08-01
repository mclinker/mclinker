/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TYPE_TRAITS_H
#define MCLD_TYPE_TRAITS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{
template<typename DataType>
struct NonConstTraits;

template<typename DataType>
struct ConstTraits
{
  typedef DataType                 value_type;
  typedef const DataType*          pointer;
  typedef const DataType&          reference;
  typedef size_t                   size_type;
  typedef ConstTraits<DataType>    const_traits;
  typedef NonConstTraits<DataType> nonconst_traits;
};

template<typename DataType>
struct NonConstTraits
{
  typedef DataType                 value_type;
  typedef DataType*                pointer;
  typedef DataType&                reference;
  typedef size_t                   size_type;
  typedef ConstTraits<DataType>    const_traits;
  typedef NonConstTraits<DataType> nonconst_traits;
};

} // namespace of mcld

#endif

