
// Generated from KnoBABQuery.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "KnoBABQueryVisitor.h"


/**
 * This class provides an empty implementation of KnoBABQueryVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  KnoBABQueryBaseVisitor : public KnoBABQueryVisitor {
public:

  virtual std::any visitQueries(KnoBABQueryParser::QueriesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSet_benchmarking_file(KnoBABQueryParser::Set_benchmarking_fileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLoad_data_query(KnoBABQueryParser::Load_data_queryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDisplay(KnoBABQueryParser::DisplayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList(KnoBABQueryParser::ListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLog(KnoBABQueryParser::LogContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrace(KnoBABQueryParser::TraceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent(KnoBABQueryParser::EventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_part(KnoBABQueryParser::Data_partContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitField(KnoBABQueryParser::FieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModel_query(KnoBABQueryParser::Model_queryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFile_model(KnoBABQueryParser::File_modelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclares(KnoBABQueryParser::DeclaresContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopn(KnoBABQueryParser::TopnContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtomization(KnoBABQueryParser::AtomizationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGrounding(KnoBABQueryParser::GroundingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDisplay_qp(KnoBABQueryParser::Display_qpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQuery_plan(KnoBABQueryParser::Query_planContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclare_syntax(KnoBABQueryParser::Declare_syntaxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHas_args(KnoBABQueryParser::Has_argsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNext(KnoBABQueryParser::NextContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInit(KnoBABQueryParser::InitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOr(KnoBABQueryParser::OrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLast(KnoBABQueryParser::LastContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAbsence(KnoBABQueryParser::AbsenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBox(KnoBABQueryParser::BoxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiamond(KnoBABQueryParser::DiamondContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNot(KnoBABQueryParser::NotContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParen(KnoBABQueryParser::ParenContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnd_next_globally(KnoBABQueryParser::And_next_globallyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnd(KnoBABQueryParser::AndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImplication(KnoBABQueryParser::ImplicationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnd_globally(KnoBABQueryParser::And_globallyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExists(KnoBABQueryParser::ExistsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnd(KnoBABQueryParser::EndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUntil(KnoBABQueryParser::UntilContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnd_future(KnoBABQueryParser::And_futureContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFirst(KnoBABQueryParser::FirstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfte(KnoBABQueryParser::IfteContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_aware_declare(KnoBABQueryParser::Data_aware_declareContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNary_prop(KnoBABQueryParser::Nary_propContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_prop(KnoBABQueryParser::Unary_propContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFields(KnoBABQueryParser::FieldsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDisj(KnoBABQueryParser::DisjContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConj_or_atom(KnoBABQueryParser::Conj_or_atomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTop(KnoBABQueryParser::TopContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIn_atom(KnoBABQueryParser::In_atomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtom_conj(KnoBABQueryParser::Atom_conjContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtom(KnoBABQueryParser::AtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLt(KnoBABQueryParser::LtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLeq(KnoBABQueryParser::LeqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGt(KnoBABQueryParser::GtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeq(KnoBABQueryParser::GeqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEq(KnoBABQueryParser::EqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNeq(KnoBABQueryParser::NeqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclare_arguments(KnoBABQueryParser::Declare_argumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclare_act_target(KnoBABQueryParser::Declare_act_targetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNo_preliminary_fill(KnoBABQueryParser::No_preliminary_fillContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAct_for_attributes(KnoBABQueryParser::Act_for_attributesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNo_cream_off(KnoBABQueryParser::No_cream_offContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWith_data(KnoBABQueryParser::With_dataContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWith_missing(KnoBABQueryParser::With_missingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNo_stats(KnoBABQueryParser::No_statsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVar(KnoBABQueryParser::VarContext *ctx) override {
    return visitChildren(ctx);
  }


};

