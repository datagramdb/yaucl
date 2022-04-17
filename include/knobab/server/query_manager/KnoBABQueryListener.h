
// Generated from KnoBABQuery.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "KnoBABQueryParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by KnoBABQueryParser.
 */
class  KnoBABQueryListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterQueries(KnoBABQueryParser::QueriesContext *ctx) = 0;
  virtual void exitQueries(KnoBABQueryParser::QueriesContext *ctx) = 0;

  virtual void enterSet_benchmarking_file(KnoBABQueryParser::Set_benchmarking_fileContext *ctx) = 0;
  virtual void exitSet_benchmarking_file(KnoBABQueryParser::Set_benchmarking_fileContext *ctx) = 0;

  virtual void enterLoad_data_query(KnoBABQueryParser::Load_data_queryContext *ctx) = 0;
  virtual void exitLoad_data_query(KnoBABQueryParser::Load_data_queryContext *ctx) = 0;

  virtual void enterDisplay(KnoBABQueryParser::DisplayContext *ctx) = 0;
  virtual void exitDisplay(KnoBABQueryParser::DisplayContext *ctx) = 0;

  virtual void enterList(KnoBABQueryParser::ListContext *ctx) = 0;
  virtual void exitList(KnoBABQueryParser::ListContext *ctx) = 0;

  virtual void enterLog(KnoBABQueryParser::LogContext *ctx) = 0;
  virtual void exitLog(KnoBABQueryParser::LogContext *ctx) = 0;

  virtual void enterTrace(KnoBABQueryParser::TraceContext *ctx) = 0;
  virtual void exitTrace(KnoBABQueryParser::TraceContext *ctx) = 0;

  virtual void enterEvent(KnoBABQueryParser::EventContext *ctx) = 0;
  virtual void exitEvent(KnoBABQueryParser::EventContext *ctx) = 0;

  virtual void enterData_part(KnoBABQueryParser::Data_partContext *ctx) = 0;
  virtual void exitData_part(KnoBABQueryParser::Data_partContext *ctx) = 0;

  virtual void enterField(KnoBABQueryParser::FieldContext *ctx) = 0;
  virtual void exitField(KnoBABQueryParser::FieldContext *ctx) = 0;

  virtual void enterModel_query(KnoBABQueryParser::Model_queryContext *ctx) = 0;
  virtual void exitModel_query(KnoBABQueryParser::Model_queryContext *ctx) = 0;

  virtual void enterFile_model(KnoBABQueryParser::File_modelContext *ctx) = 0;
  virtual void exitFile_model(KnoBABQueryParser::File_modelContext *ctx) = 0;

  virtual void enterDeclares(KnoBABQueryParser::DeclaresContext *ctx) = 0;
  virtual void exitDeclares(KnoBABQueryParser::DeclaresContext *ctx) = 0;

  virtual void enterTopn(KnoBABQueryParser::TopnContext *ctx) = 0;
  virtual void exitTopn(KnoBABQueryParser::TopnContext *ctx) = 0;

  virtual void enterAtomization(KnoBABQueryParser::AtomizationContext *ctx) = 0;
  virtual void exitAtomization(KnoBABQueryParser::AtomizationContext *ctx) = 0;

  virtual void enterGrounding(KnoBABQueryParser::GroundingContext *ctx) = 0;
  virtual void exitGrounding(KnoBABQueryParser::GroundingContext *ctx) = 0;

  virtual void enterDisplay_qp(KnoBABQueryParser::Display_qpContext *ctx) = 0;
  virtual void exitDisplay_qp(KnoBABQueryParser::Display_qpContext *ctx) = 0;

  virtual void enterQuery_plan(KnoBABQueryParser::Query_planContext *ctx) = 0;
  virtual void exitQuery_plan(KnoBABQueryParser::Query_planContext *ctx) = 0;

  virtual void enterDeclare_syntax(KnoBABQueryParser::Declare_syntaxContext *ctx) = 0;
  virtual void exitDeclare_syntax(KnoBABQueryParser::Declare_syntaxContext *ctx) = 0;

  virtual void enterHas_args(KnoBABQueryParser::Has_argsContext *ctx) = 0;
  virtual void exitHas_args(KnoBABQueryParser::Has_argsContext *ctx) = 0;

  virtual void enterNext(KnoBABQueryParser::NextContext *ctx) = 0;
  virtual void exitNext(KnoBABQueryParser::NextContext *ctx) = 0;

  virtual void enterInit(KnoBABQueryParser::InitContext *ctx) = 0;
  virtual void exitInit(KnoBABQueryParser::InitContext *ctx) = 0;

  virtual void enterOr(KnoBABQueryParser::OrContext *ctx) = 0;
  virtual void exitOr(KnoBABQueryParser::OrContext *ctx) = 0;

  virtual void enterLast(KnoBABQueryParser::LastContext *ctx) = 0;
  virtual void exitLast(KnoBABQueryParser::LastContext *ctx) = 0;

  virtual void enterAbsence(KnoBABQueryParser::AbsenceContext *ctx) = 0;
  virtual void exitAbsence(KnoBABQueryParser::AbsenceContext *ctx) = 0;

  virtual void enterBox(KnoBABQueryParser::BoxContext *ctx) = 0;
  virtual void exitBox(KnoBABQueryParser::BoxContext *ctx) = 0;

  virtual void enterDiamond(KnoBABQueryParser::DiamondContext *ctx) = 0;
  virtual void exitDiamond(KnoBABQueryParser::DiamondContext *ctx) = 0;

  virtual void enterNot(KnoBABQueryParser::NotContext *ctx) = 0;
  virtual void exitNot(KnoBABQueryParser::NotContext *ctx) = 0;

  virtual void enterParen(KnoBABQueryParser::ParenContext *ctx) = 0;
  virtual void exitParen(KnoBABQueryParser::ParenContext *ctx) = 0;

  virtual void enterAnd_next_globally(KnoBABQueryParser::And_next_globallyContext *ctx) = 0;
  virtual void exitAnd_next_globally(KnoBABQueryParser::And_next_globallyContext *ctx) = 0;

  virtual void enterAnd(KnoBABQueryParser::AndContext *ctx) = 0;
  virtual void exitAnd(KnoBABQueryParser::AndContext *ctx) = 0;

  virtual void enterImplication(KnoBABQueryParser::ImplicationContext *ctx) = 0;
  virtual void exitImplication(KnoBABQueryParser::ImplicationContext *ctx) = 0;

  virtual void enterAnd_globally(KnoBABQueryParser::And_globallyContext *ctx) = 0;
  virtual void exitAnd_globally(KnoBABQueryParser::And_globallyContext *ctx) = 0;

  virtual void enterExists(KnoBABQueryParser::ExistsContext *ctx) = 0;
  virtual void exitExists(KnoBABQueryParser::ExistsContext *ctx) = 0;

  virtual void enterEnd(KnoBABQueryParser::EndContext *ctx) = 0;
  virtual void exitEnd(KnoBABQueryParser::EndContext *ctx) = 0;

  virtual void enterUntil(KnoBABQueryParser::UntilContext *ctx) = 0;
  virtual void exitUntil(KnoBABQueryParser::UntilContext *ctx) = 0;

  virtual void enterAnd_future(KnoBABQueryParser::And_futureContext *ctx) = 0;
  virtual void exitAnd_future(KnoBABQueryParser::And_futureContext *ctx) = 0;

  virtual void enterFirst(KnoBABQueryParser::FirstContext *ctx) = 0;
  virtual void exitFirst(KnoBABQueryParser::FirstContext *ctx) = 0;

  virtual void enterIfte(KnoBABQueryParser::IfteContext *ctx) = 0;
  virtual void exitIfte(KnoBABQueryParser::IfteContext *ctx) = 0;

  virtual void enterData_aware_declare(KnoBABQueryParser::Data_aware_declareContext *ctx) = 0;
  virtual void exitData_aware_declare(KnoBABQueryParser::Data_aware_declareContext *ctx) = 0;

  virtual void enterNary_prop(KnoBABQueryParser::Nary_propContext *ctx) = 0;
  virtual void exitNary_prop(KnoBABQueryParser::Nary_propContext *ctx) = 0;

  virtual void enterUnary_prop(KnoBABQueryParser::Unary_propContext *ctx) = 0;
  virtual void exitUnary_prop(KnoBABQueryParser::Unary_propContext *ctx) = 0;

  virtual void enterFields(KnoBABQueryParser::FieldsContext *ctx) = 0;
  virtual void exitFields(KnoBABQueryParser::FieldsContext *ctx) = 0;

  virtual void enterDisj(KnoBABQueryParser::DisjContext *ctx) = 0;
  virtual void exitDisj(KnoBABQueryParser::DisjContext *ctx) = 0;

  virtual void enterConj_or_atom(KnoBABQueryParser::Conj_or_atomContext *ctx) = 0;
  virtual void exitConj_or_atom(KnoBABQueryParser::Conj_or_atomContext *ctx) = 0;

  virtual void enterTop(KnoBABQueryParser::TopContext *ctx) = 0;
  virtual void exitTop(KnoBABQueryParser::TopContext *ctx) = 0;

  virtual void enterIn_atom(KnoBABQueryParser::In_atomContext *ctx) = 0;
  virtual void exitIn_atom(KnoBABQueryParser::In_atomContext *ctx) = 0;

  virtual void enterAtom_conj(KnoBABQueryParser::Atom_conjContext *ctx) = 0;
  virtual void exitAtom_conj(KnoBABQueryParser::Atom_conjContext *ctx) = 0;

  virtual void enterAtom(KnoBABQueryParser::AtomContext *ctx) = 0;
  virtual void exitAtom(KnoBABQueryParser::AtomContext *ctx) = 0;

  virtual void enterLt(KnoBABQueryParser::LtContext *ctx) = 0;
  virtual void exitLt(KnoBABQueryParser::LtContext *ctx) = 0;

  virtual void enterLeq(KnoBABQueryParser::LeqContext *ctx) = 0;
  virtual void exitLeq(KnoBABQueryParser::LeqContext *ctx) = 0;

  virtual void enterGt(KnoBABQueryParser::GtContext *ctx) = 0;
  virtual void exitGt(KnoBABQueryParser::GtContext *ctx) = 0;

  virtual void enterGeq(KnoBABQueryParser::GeqContext *ctx) = 0;
  virtual void exitGeq(KnoBABQueryParser::GeqContext *ctx) = 0;

  virtual void enterEq(KnoBABQueryParser::EqContext *ctx) = 0;
  virtual void exitEq(KnoBABQueryParser::EqContext *ctx) = 0;

  virtual void enterNeq(KnoBABQueryParser::NeqContext *ctx) = 0;
  virtual void exitNeq(KnoBABQueryParser::NeqContext *ctx) = 0;

  virtual void enterDeclare_arguments(KnoBABQueryParser::Declare_argumentsContext *ctx) = 0;
  virtual void exitDeclare_arguments(KnoBABQueryParser::Declare_argumentsContext *ctx) = 0;

  virtual void enterDeclare_act_target(KnoBABQueryParser::Declare_act_targetContext *ctx) = 0;
  virtual void exitDeclare_act_target(KnoBABQueryParser::Declare_act_targetContext *ctx) = 0;

  virtual void enterNo_preliminary_fill(KnoBABQueryParser::No_preliminary_fillContext *ctx) = 0;
  virtual void exitNo_preliminary_fill(KnoBABQueryParser::No_preliminary_fillContext *ctx) = 0;

  virtual void enterAct_for_attributes(KnoBABQueryParser::Act_for_attributesContext *ctx) = 0;
  virtual void exitAct_for_attributes(KnoBABQueryParser::Act_for_attributesContext *ctx) = 0;

  virtual void enterNo_cream_off(KnoBABQueryParser::No_cream_offContext *ctx) = 0;
  virtual void exitNo_cream_off(KnoBABQueryParser::No_cream_offContext *ctx) = 0;

  virtual void enterWith_data(KnoBABQueryParser::With_dataContext *ctx) = 0;
  virtual void exitWith_data(KnoBABQueryParser::With_dataContext *ctx) = 0;

  virtual void enterWith_missing(KnoBABQueryParser::With_missingContext *ctx) = 0;
  virtual void exitWith_missing(KnoBABQueryParser::With_missingContext *ctx) = 0;

  virtual void enterNo_stats(KnoBABQueryParser::No_statsContext *ctx) = 0;
  virtual void exitNo_stats(KnoBABQueryParser::No_statsContext *ctx) = 0;

  virtual void enterVar(KnoBABQueryParser::VarContext *ctx) = 0;
  virtual void exitVar(KnoBABQueryParser::VarContext *ctx) = 0;


};

