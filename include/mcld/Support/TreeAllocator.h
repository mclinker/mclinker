/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TREE_ALLOCATOR_H
#define MCLD_TREE_ALLOCATOR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <set>
#include <mcld/Support/TreeBase.h>

namespace mcld
{

template<typename DataType, typename DataAlloc = std::allocator<DataType> >
class NodeAllocator
{
public:
  typedef size_t          size_type;
  typedef ptrdiff_t       difference_type;
  typedef Node<DataType>  node_type;
  typedef DataType        value_type;
  typedef DataType*       value_ptr;
  typedef DataType&       value_reference;

  typedef DataAlloc                          DataAllocType;
  typedef NodeAllocator<DataType, DataAlloc> NodeAllocType;

private:
  typedef std::multiset<node_type*> NodeListType;

public:
  NodeAllocator() {
  }

  NodeAllocator(const NodeAllocator& pCopy) 
    : m_NodeList(pCopy.m_NodeList) {
  }

  ~NodeAllocator() {
     clear();
  }

  node_type* allocate(size_type N) {
    value_type *data = m_DataAlloc.allocate(1);
    node_type *result = new node_type();
    result->data = data;
    m_NodeList.insert(result);
    return result;
  }

  void deallocate(node_type*& pNode) {
    m_DataAlloc.deallocate(pNode->data, 1);
    typename NodeListType::iterator position = m_NodeList.find(pNode);
    m_NodeList.erase(position);
    delete pNode;
    pNode = 0;
  }

  void clear() {
    typename NodeListType::iterator node;
    typename NodeListType::iterator nodeEnd = m_NodeList.end();
    for (node = m_NodeList.begin(); node!=nodeEnd; ++node) {
      m_DataAlloc.deallocate(&(*node)->data, 1);
      delete (*node);
    }
    m_NodeList.clear();
  }

  unsigned int size() const {
    return m_NodeList.size();
  }

  bool empty() const {
    return m_NodeList.empty();
  }

  void construct(value_ptr& pDataPtr, const value_reference pValue) {
    node_type *node = allocate(1);
    m_DataAlloc.construct(node->data, pValue);
    pDataPtr = node->data;
  }

  void destroy(value_ptr pDataPtr) {
    m_DataAlloc.destroy(pDataPtr);
  }
private:
  NodeListType m_NodeList;
  DataAlloc m_DataAlloc;
};

} // namespace of mcld

#endif

