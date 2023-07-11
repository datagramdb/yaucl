//
// Created by giacomo on 23/01/23.
//

#ifndef KNOBAB_SERVER_PARSEFFLOATDOT_H
#define KNOBAB_SERVER_PARSEFFLOATDOT_H

namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
}
#include <yaucl/graphs/dot/DOTVisitor.h>
#include <sstream>
#include <fstream>
#ifndef CLASSIFIERS_PAIR_HASH_H
#define CLASSIFIERS_PAIR_HASH_H
#define CLASSIFIERS_PAIR_HASH_H_redef
#endif
#include <iostream>
#include <yaucl/graphs/NodeLabelBijectionFA.h>
#include <yaucl/graphs/FlexibleFA.h>
#include <yaucl/bpm/structures/ltlf/ltlf.h>
#include <yaucl/bpm/structures/commons/easy_prop.h>

class ParseFFLOATDot : public DOTVisitor {

    NodeLabelBijectionFA<std::string, easy_prop> parsing_result;
public:

    ///bool need_back_conversion;
    ///const std::unordered_map<std::string, std::string>* back_conv;

    ParseFFLOATDot() /*: back_conv{nullptr}, need_back_conversion{false}*/ {}
    ~ParseFFLOATDot()  {}

    NodeLabelBijectionFA<std::string, easy_prop> parse(std::istream &stream);


    antlrcpp::Any visitGraph(DOTParser::GraphContext *context) override;
    antlrcpp::Any visitStmt_list(DOTParser::Stmt_listContext *context) override;
    antlrcpp::Any visitStmt(DOTParser::StmtContext *context) override;
    antlrcpp::Any visitAttr_stmt(DOTParser::Attr_stmtContext *context) override;
    antlrcpp::Any visitAttr_list(DOTParser::Attr_listContext *context) override;
    antlrcpp::Any visitA_list(DOTParser::A_listContext *context) override;
    antlrcpp::Any visitEdge_stmt(DOTParser::Edge_stmtContext *context) override;
    antlrcpp::Any visitEdgeRHS(DOTParser::EdgeRHSContext *context) override;
    antlrcpp::Any visitEdgeop(DOTParser::EdgeopContext *context) override;
    antlrcpp::Any visitNode_stmt(DOTParser::Node_stmtContext *context) override;
    antlrcpp::Any visitNode_id(DOTParser::Node_idContext *context) override;
    antlrcpp::Any visitPort(DOTParser::PortContext *context) override;
    antlrcpp::Any visitSubgraph(DOTParser::SubgraphContext *context) override;
    antlrcpp::Any visitId(DOTParser::IdContext *context) override;
};
#ifdef CLASSIFIERS_PAIR_HASH_H_redef
#undef CLASSIFIERS_PAIR_HASH_H_redef
#endif

#endif //KNOBAB_SERVER_PARSEFFLOATDOT_H
