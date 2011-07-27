/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TREE_BASE
#define MCLD_TREE_BASE

template<typename DataType>
struct NonConstTraits;

template<typename DataType>
struct ConstTraits
{
  typedef DataType                 value_type;
  typedef size_t                   size_type;
  typedef ConstTraits<DataType>    const_traits;
  typedef NonConstTraits<DataType> nonconst_traits;
};

template<typename DataType>
struct NonConstTraits
{
  typedef DataType                 value_type;
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

#endif

