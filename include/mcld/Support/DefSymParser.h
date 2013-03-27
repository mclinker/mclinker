#ifndef MCLD_DEFSYM_PARSER
#define MCLD_DEFSYM_PARSER

#include <mcld/LD/LDSymbol.h>
#include <mcld/Module.h>
#include <mcld/Support/MsgHandling.h>

#include <llvm/ADT/StringRef.h>

namespace mcld {
class DefSymParser
{
public:
  DefSymParser(Module* pModule);

  ~DefSymParser();
  // passing a valid operator will return a number whose quantity relative
  // to other such obtained quantities will give the priority of the operator
  int precedence(const char* x);

  //parse a valid expression and set the value in the second paramter
  bool parse(llvm::StringRef, uint64_t&);

private:
  mcld::Module* m_pModule;
};

} // mcld
#endif
