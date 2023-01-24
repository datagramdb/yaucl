
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "FLLOATPropListener.h"


/**
 * This class provides an empty implementation of FLLOATPropListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  FLLOATPropBaseListener : public FLLOATPropListener {
public:

  virtual void enterNegation(FLLOATPropParser::NegationContext * /*ctx*/) override { }
  virtual void exitNegation(FLLOATPropParser::NegationContext * /*ctx*/) override { }

  virtual void enterParen(FLLOATPropParser::ParenContext * /*ctx*/) override { }
  virtual void exitParen(FLLOATPropParser::ParenContext * /*ctx*/) override { }

  virtual void enterOr(FLLOATPropParser::OrContext * /*ctx*/) override { }
  virtual void exitOr(FLLOATPropParser::OrContext * /*ctx*/) override { }

  virtual void enterTop(FLLOATPropParser::TopContext * /*ctx*/) override { }
  virtual void exitTop(FLLOATPropParser::TopContext * /*ctx*/) override { }

  virtual void enterBot(FLLOATPropParser::BotContext * /*ctx*/) override { }
  virtual void exitBot(FLLOATPropParser::BotContext * /*ctx*/) override { }

  virtual void enterAnd(FLLOATPropParser::AndContext * /*ctx*/) override { }
  virtual void exitAnd(FLLOATPropParser::AndContext * /*ctx*/) override { }

  virtual void enterAtom(FLLOATPropParser::AtomContext * /*ctx*/) override { }
  virtual void exitAtom(FLLOATPropParser::AtomContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

