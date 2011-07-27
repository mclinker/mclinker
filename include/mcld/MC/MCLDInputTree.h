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
class InputTree
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

public:
  class iterator {
  };

  class const_iterator {
  };

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

  // -----  iterator  -----
  friend class iterator;
  iterator root();
  iterator begin();
  iterator end();
  const_iterator root() const;
  const_iterator begin() const;
  const_iterator end() const;

  // -----  observers -----
  unsigned int size() const;
  bool empty() const;

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

