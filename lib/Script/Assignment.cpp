//===- Assignment.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/Assignment.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/Operand.h>
#include <mcld/Script/Operator.h>
#include <mcld/Script/RpnEvaluator.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <llvm/Support/Casting.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Assignment
//===----------------------------------------------------------------------===//
Assignment::Assignment(const Module& pModule,
                       const TargetLDBackend& pLDBackend,
                       LinkerScript& pScript,
                       Level pLevel,
                       Type pType,
                       SymOperand& pSymbol,
                       RpnExpr& pRpnExpr)
  : ScriptCommand(ScriptCommand::ASSIGNMENT),
    m_Module(pModule),
    m_LDBackend(pLDBackend),
    m_Script(pScript),
    m_Level(pLevel),
    m_Type(pType),
    m_Symbol(pSymbol),
    m_RpnExpr(pRpnExpr)
{
}

Assignment::~Assignment()
{
}

Assignment& Assignment::operator=(const Assignment& pAssignment)
{
  return *this;
}

void Assignment::dump() const
{
  switch (type()) {
  case DEFAULT:
    break;
  case HIDDEN:
    mcld::outs() << "HIDDEN ( ";
    break;
  case PROVIDE:
    mcld::outs() << "PROVIDE ( ";
    break;
  case PROVIDE_HIDDEN:
    mcld::outs() << "PROVIDE_HIDDEN ( ";
    break;
  default:
    break;
  }

  m_Symbol.dump();

  mcld::outs() << " = ";

  m_RpnExpr.dump();

  if (type() != DEFAULT)
    mcld::outs() << " )";

  mcld::outs() << ";\n";
}

void Assignment::activate()
{
  bool isLhsDot = m_Symbol.isDot();
  switch (m_Level) {
  case OUTSIDES_SECTIONS:
    assert(!isLhsDot);
    m_Script.assignments().push_back(std::make_pair((LDSymbol*)NULL, *this));
    break;

  case OUTPUT_SECTION: {
    bool hasDotInRhs = m_RpnExpr.hasDot();
    SectionMap::reference out = m_Script.sectionMap().back();
    if (hasDotInRhs) {
      if (out->dotAssignments().empty()) {
        SectionMap::iterator prev = m_Script.sectionMap().begin() +
                                    m_Script.sectionMap().size() - 2;
        // . = ADDR ( `output_sect' ) + SIZEOF ( `output_sect' )
        SymOperand* dot = SymOperand::create(".");
        RpnExpr* expr = RpnExpr::create();
        expr->push_back(SectDescOperand::create(*prev));
        expr->push_back(
          &Operator::create<Operator::ADDR>(m_Module, m_LDBackend));
        expr->push_back(SectDescOperand::create(*prev));
        expr->push_back(
          &Operator::create<Operator::SIZEOF>(m_Module, m_LDBackend));
        expr->push_back(
          &Operator::create<Operator::ADD>(m_Module, m_LDBackend));
        Assignment assign(m_Module, m_LDBackend, m_Script, OUTPUT_SECTION,
                          DEFAULT, *dot, *expr);
        out->dotAssignments().push_back(assign);
      }

      for (RpnExpr::iterator it = m_RpnExpr.begin(), ie = m_RpnExpr.end();
        it != ie; ++it) {
        if ((*it)->kind() == ExprToken::OPERAND &&
            llvm::cast<Operand>(*it)->isDot())
          (*it) = &(out->dotAssignments().back().symbol());
      } // for each expression token
    }

    if (isLhsDot) {
      out->dotAssignments().push_back(*this);
    } else {
      m_Script.assignments().push_back(std::make_pair((LDSymbol*)NULL, *this));
    }

    break;
  }

  case INPUT_SECTION: {
    bool hasDotInRhs = m_RpnExpr.hasDot();
    SectionMap::Output::reference in = m_Script.sectionMap().back()->back();
    if (hasDotInRhs) {
      if (in->dotAssignments().empty()) {
        // . = `frag'
        SymOperand* dot = SymOperand::create(".");
        RpnExpr* expr = RpnExpr::create();
        expr->push_back(
          FragOperand::create(in->getSection()->getSectionData()->front()));
        Assignment assign(m_Module, m_LDBackend, m_Script, INPUT_SECTION,
                          DEFAULT, *dot, *expr);
        in->dotAssignments().push_back(std::make_pair((Fragment*)NULL, assign));
      }

      for (RpnExpr::iterator it = m_RpnExpr.begin(), ie = m_RpnExpr.end();
        it != ie; ++it) {
        if ((*it)->kind() == ExprToken::OPERAND &&
            llvm::cast<Operand>(*it)->isDot())
          (*it) = &(in->dotAssignments().back().second.symbol());
      } // end of for
    }

    if (isLhsDot) {
      in->dotAssignments().push_back(
        std::make_pair(in->getSection()->getSectionData()->front().getNextNode(),
                       *this));
    } else {
      m_Script.assignments().push_back(std::make_pair((LDSymbol*)NULL, *this));
    }

    break;
  }

  } // end of switch
}

bool Assignment::assign()
{
  RpnEvaluator evaluator(m_Module);
  uint64_t result = 0;
  bool success = evaluator.eval(m_RpnExpr, result);
  if (success)
    m_Symbol.setValue(result);
  return success;
}
