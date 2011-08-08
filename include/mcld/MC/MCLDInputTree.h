/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLD_INPUTREE_H
#define MCLD_INPUTREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/FileSystem.h>
#include <mcld/ADT/BinTree.h>
#include <mcld/MC/MCLDFile.h>
#include <string>

namespace mcld
{

/** \class InputTree
 *  \brief InputTree is the input tree to contains all inputs from the
 *  command line.
 *
 *  InputTree, of course, is uncopyable.
 *
 *  @see MCLDFile
 */
class InputTree : public BinaryTree<MCLDFile> 
{
public:
  enum Direction {
    Positional = TreeIteratorBase::Rightward,
    Inclusive  = TreeIteratorBase::Leftward
  };

  enum InputType {
    Archive = MCLDFile::Archive,
    Object = MCLDFile::Object,
    DynObj = MCLDFile::DynObj,
    Script = MCLDFile::Script,
    Input = MCLDFile::Unknown
  };

  typedef BinaryTree<MCLDFile>::iterator       iterator;
  typedef BinaryTree<MCLDFile>::const_iterator const_iterator;

public:
  struct Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const = 0;
  };

  struct Succeeder : public Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const {
      proxy::hook<Positional>(pFrom.m_pNode, pTo.m_pNode);
    }
  };

  struct Includer : public Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const {
      proxy::hook<Inclusive>(pFrom.m_pNode, pTo.m_pNode);
    }
  };

public:
  static Succeeder Afterward;
  static Includer  Downward;

public:
  InputTree();
  ~InputTree();

  // -----  modify  ----- //
  /// insert - create a leaf node and merge it in the tree.
  //  This version of join determines the direction at run time.
  //  @param position the parent node
  //  @param value the value being pushed.
  //  @param pConnector the direction of the connecting edge of the parent node.

  template<size_t DIRECT>
  InputTree& insert(iterator pPosition,
                    InputType pInputType,
                    const std::string& pNamespec,
                    const sys::fs::Path& pPath);

  template<size_t DIRECT>
  InputTree& enterGroup(iterator pPosition);


  InputTree& merge(iterator pPosition,
                   InputTree& pTree,
                   const Connector& pConnector);

  InputTree& insert(iterator pPosition,
                    InputType pInputType,
                    const std::string& pNamespec,
                    const sys::fs::Path& pPath,
                    const Connector& pConnector);

  InputTree& enterGroup(iterator pPosition,
                        const Connector& pConnector);

  // -----  observers  ----- //
  unsigned int fileSize() const
  { return m_FileFactory.size(); }

  bool hasFiles() const
  { return !m_FileFactory.empty(); }

private:
  MCLDFileFactory<64> m_FileFactory;

};

bool isGroup(const InputTree::iterator& pos);
bool isGroup(const InputTree::const_iterator& pos);

} // namespace of mcld

#endif

