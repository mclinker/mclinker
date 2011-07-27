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
#include <mcld/Support/BinTree.h>
#include <mcld/MC/MCLDFile.h>
#include <string>

namespace mcld
{
class MCLDFile;

/** \class InputTree
 *  \brief InputTree is the input tree to contains all inputs from the
 *  command line.
 *
 *  @see MCLDFile
 */
class InputTree : public BinTree<MCLDFile> 
{
public:
  enum Direction {
    Positional,
    Inclusive
  };

  enum InputType {
    Archive,
    Object,
    Script,
    Group,
    Input
  };

  typedef BinTree<MCLDFile>::iterator       iterator;
  typedef BinTree<MCLDFile>::const_iterator const_iterator;
public:

  class Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const = 0;
  };

  class Succeeder : public Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const {
    }
  };

  class Includer : public Connector {
    virtual void connect(iterator& pFrom, const const_iterator& pTo) const {
    }
  };
private:
  InputTree(const InputTree &); // DO NOT IMPLEMENT
  void operator=(const InputTree &); // DO NOT IMPLEMENT

public:
  InputTree();
  ~InputTree();

  // -----  modify  -----
  InputTree& insert(iterator pPosition,
                    InputType pInputType,
                    const std::string& pNamespec,
                    const sys::fs::Path& pPath,
                    const Connector& pConnector);

  InputTree& enterGroup(iterator pPosition,
                        const Connector& pConnector);
};

} // namespace of mcld

#endif

