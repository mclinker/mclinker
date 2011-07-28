/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TREE_BASE_H
#define MCLD_TREE_BASE_H

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

struct NodeBase
{
  NodeBase *left;
  NodeBase *right;
};

template<typename DataType>
struct Node
{
public:
  typedef DataType value_type;

public:
  value_type data;
};

} // namespace of mcld

#endif

