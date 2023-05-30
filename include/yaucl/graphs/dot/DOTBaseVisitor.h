
// Generated from include/yaucl/graphs/DOT.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "DOTVisitor.h"


/**
 * This class provides an empty implementation of DOTVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  DOTBaseVisitor : public DOTVisitor {
public:

  virtual std::any visitGraph(DOTParser::GraphContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStmt_list(DOTParser::Stmt_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStmt(DOTParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttr_stmt(DOTParser::Attr_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttr_list(DOTParser::Attr_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitA_list(DOTParser::A_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdge_stmt(DOTParser::Edge_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdgeRHS(DOTParser::EdgeRHSContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdgeop(DOTParser::EdgeopContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNode_stmt(DOTParser::Node_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNode_id(DOTParser::Node_idContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort(DOTParser::PortContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSubgraph(DOTParser::SubgraphContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitId(DOTParser::IdContext *ctx) override {
    return visitChildren(ctx);
  }


};

