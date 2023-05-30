
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "FLLOATPropParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by FLLOATPropParser.
 */
class  FLLOATPropListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterNegation(FLLOATPropParser::NegationContext *ctx) = 0;
  virtual void exitNegation(FLLOATPropParser::NegationContext *ctx) = 0;

  virtual void enterParen(FLLOATPropParser::ParenContext *ctx) = 0;
  virtual void exitParen(FLLOATPropParser::ParenContext *ctx) = 0;

  virtual void enterOr(FLLOATPropParser::OrContext *ctx) = 0;
  virtual void exitOr(FLLOATPropParser::OrContext *ctx) = 0;

  virtual void enterTop(FLLOATPropParser::TopContext *ctx) = 0;
  virtual void exitTop(FLLOATPropParser::TopContext *ctx) = 0;

  virtual void enterBot(FLLOATPropParser::BotContext *ctx) = 0;
  virtual void exitBot(FLLOATPropParser::BotContext *ctx) = 0;

  virtual void enterAnd(FLLOATPropParser::AndContext *ctx) = 0;
  virtual void exitAnd(FLLOATPropParser::AndContext *ctx) = 0;

  virtual void enterAtom(FLLOATPropParser::AtomContext *ctx) = 0;
  virtual void exitAtom(FLLOATPropParser::AtomContext *ctx) = 0;


};

