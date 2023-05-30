
// Generated from include/yaucl/graphs/DOT.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "DOTParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by DOTParser.
 */
class  DOTVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by DOTParser.
   */
    virtual std::any visitGraph(DOTParser::GraphContext *context) = 0;

    virtual std::any visitStmt_list(DOTParser::Stmt_listContext *context) = 0;

    virtual std::any visitStmt(DOTParser::StmtContext *context) = 0;

    virtual std::any visitAttr_stmt(DOTParser::Attr_stmtContext *context) = 0;

    virtual std::any visitAttr_list(DOTParser::Attr_listContext *context) = 0;

    virtual std::any visitA_list(DOTParser::A_listContext *context) = 0;

    virtual std::any visitEdge_stmt(DOTParser::Edge_stmtContext *context) = 0;

    virtual std::any visitEdgeRHS(DOTParser::EdgeRHSContext *context) = 0;

    virtual std::any visitEdgeop(DOTParser::EdgeopContext *context) = 0;

    virtual std::any visitNode_stmt(DOTParser::Node_stmtContext *context) = 0;

    virtual std::any visitNode_id(DOTParser::Node_idContext *context) = 0;

    virtual std::any visitPort(DOTParser::PortContext *context) = 0;

    virtual std::any visitSubgraph(DOTParser::SubgraphContext *context) = 0;

    virtual std::any visitId(DOTParser::IdContext *context) = 0;


};

