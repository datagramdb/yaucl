
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "FLLOATPropVisitor.h"


/**
 * This class provides an empty implementation of FLLOATPropVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  FLLOATPropBaseVisitor : public FLLOATPropVisitor {
public:

  virtual std::any visitNegation(FLLOATPropParser::NegationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParen(FLLOATPropParser::ParenContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOr(FLLOATPropParser::OrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTop(FLLOATPropParser::TopContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBot(FLLOATPropParser::BotContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnd(FLLOATPropParser::AndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtom(FLLOATPropParser::AtomContext *ctx) override {
    return visitChildren(ctx);
  }


};

