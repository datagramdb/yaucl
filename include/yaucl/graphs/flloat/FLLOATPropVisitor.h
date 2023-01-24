
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "FLLOATPropParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by FLLOATPropParser.
 */
class  FLLOATPropVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by FLLOATPropParser.
   */
    virtual std::any visitNegation(FLLOATPropParser::NegationContext *context) = 0;

    virtual std::any visitParen(FLLOATPropParser::ParenContext *context) = 0;

    virtual std::any visitOr(FLLOATPropParser::OrContext *context) = 0;

    virtual std::any visitTop(FLLOATPropParser::TopContext *context) = 0;

    virtual std::any visitBot(FLLOATPropParser::BotContext *context) = 0;

    virtual std::any visitAnd(FLLOATPropParser::AndContext *context) = 0;

    virtual std::any visitAtom(FLLOATPropParser::AtomContext *context) = 0;


};

