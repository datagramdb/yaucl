
// Generated from .\KnoBABQuery.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "KnoBABQueryParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by KnoBABQueryParser.
 */
class  KnoBABQueryVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by KnoBABQueryParser.
   */
    virtual std::any visitQueries(KnoBABQueryParser::QueriesContext *context) = 0;

    virtual std::any visitSet_benchmarking_file(KnoBABQueryParser::Set_benchmarking_fileContext *context) = 0;

    virtual std::any visitLoad_data_query(KnoBABQueryParser::Load_data_queryContext *context) = 0;

    virtual std::any visitDisplay(KnoBABQueryParser::DisplayContext *context) = 0;

    virtual std::any visitList(KnoBABQueryParser::ListContext *context) = 0;

    virtual std::any visitDroplog(KnoBABQueryParser::DroplogContext *context) = 0;

    virtual std::any visitLog(KnoBABQueryParser::LogContext *context) = 0;

    virtual std::any visitTrace(KnoBABQueryParser::TraceContext *context) = 0;

    virtual std::any visitEvent(KnoBABQueryParser::EventContext *context) = 0;

    virtual std::any visitData_part(KnoBABQueryParser::Data_partContext *context) = 0;

    virtual std::any visitField(KnoBABQueryParser::FieldContext *context) = 0;

    virtual std::any visitModel_query(KnoBABQueryParser::Model_queryContext *context) = 0;

    virtual std::any visitFile_model(KnoBABQueryParser::File_modelContext *context) = 0;

    virtual std::any visitDeclares(KnoBABQueryParser::DeclaresContext *context) = 0;

    virtual std::any visitTopn(KnoBABQueryParser::TopnContext *context) = 0;

    virtual std::any visitAtomization(KnoBABQueryParser::AtomizationContext *context) = 0;

    virtual std::any visitGrounding(KnoBABQueryParser::GroundingContext *context) = 0;

    virtual std::any visitDisplay_qp(KnoBABQueryParser::Display_qpContext *context) = 0;

    virtual std::any visitQuery_plan(KnoBABQueryParser::Query_planContext *context) = 0;

    virtual std::any visitDeclare_syntax(KnoBABQueryParser::Declare_syntaxContext *context) = 0;

    virtual std::any visitHas_args(KnoBABQueryParser::Has_argsContext *context) = 0;

    virtual std::any visitNext(KnoBABQueryParser::NextContext *context) = 0;

    virtual std::any visitInit(KnoBABQueryParser::InitContext *context) = 0;

    virtual std::any visitOr(KnoBABQueryParser::OrContext *context) = 0;

    virtual std::any visitLast(KnoBABQueryParser::LastContext *context) = 0;

    virtual std::any visitAbsence(KnoBABQueryParser::AbsenceContext *context) = 0;

    virtual std::any visitBox(KnoBABQueryParser::BoxContext *context) = 0;

    virtual std::any visitDiamond(KnoBABQueryParser::DiamondContext *context) = 0;

    virtual std::any visitNot(KnoBABQueryParser::NotContext *context) = 0;

    virtual std::any visitParen(KnoBABQueryParser::ParenContext *context) = 0;

    virtual std::any visitAnd_next_globally(KnoBABQueryParser::And_next_globallyContext *context) = 0;

    virtual std::any visitAnd(KnoBABQueryParser::AndContext *context) = 0;

    virtual std::any visitImplication(KnoBABQueryParser::ImplicationContext *context) = 0;

    virtual std::any visitAnd_globally(KnoBABQueryParser::And_globallyContext *context) = 0;

    virtual std::any visitExists(KnoBABQueryParser::ExistsContext *context) = 0;

    virtual std::any visitEnd(KnoBABQueryParser::EndContext *context) = 0;

    virtual std::any visitUntil(KnoBABQueryParser::UntilContext *context) = 0;

    virtual std::any visitAnd_future(KnoBABQueryParser::And_futureContext *context) = 0;

    virtual std::any visitFirst(KnoBABQueryParser::FirstContext *context) = 0;

    virtual std::any visitIfte(KnoBABQueryParser::IfteContext *context) = 0;

    virtual std::any visitData_aware_declare(KnoBABQueryParser::Data_aware_declareContext *context) = 0;

    virtual std::any visitNary_prop(KnoBABQueryParser::Nary_propContext *context) = 0;

    virtual std::any visitUnary_prop(KnoBABQueryParser::Unary_propContext *context) = 0;

    virtual std::any visitFields(KnoBABQueryParser::FieldsContext *context) = 0;

    virtual std::any visitDisj(KnoBABQueryParser::DisjContext *context) = 0;

    virtual std::any visitConj_or_atom(KnoBABQueryParser::Conj_or_atomContext *context) = 0;

    virtual std::any visitTop(KnoBABQueryParser::TopContext *context) = 0;

    virtual std::any visitIn_atom(KnoBABQueryParser::In_atomContext *context) = 0;

    virtual std::any visitAtom_conj(KnoBABQueryParser::Atom_conjContext *context) = 0;

    virtual std::any visitAtom(KnoBABQueryParser::AtomContext *context) = 0;

    virtual std::any visitLt(KnoBABQueryParser::LtContext *context) = 0;

    virtual std::any visitLeq(KnoBABQueryParser::LeqContext *context) = 0;

    virtual std::any visitGt(KnoBABQueryParser::GtContext *context) = 0;

    virtual std::any visitGeq(KnoBABQueryParser::GeqContext *context) = 0;

    virtual std::any visitEq(KnoBABQueryParser::EqContext *context) = 0;

    virtual std::any visitNeq(KnoBABQueryParser::NeqContext *context) = 0;

    virtual std::any visitDeclare_arguments(KnoBABQueryParser::Declare_argumentsContext *context) = 0;

    virtual std::any visitDeclare_act_target(KnoBABQueryParser::Declare_act_targetContext *context) = 0;

    virtual std::any visitNo_preliminary_fill(KnoBABQueryParser::No_preliminary_fillContext *context) = 0;

    virtual std::any visitAct_for_attributes(KnoBABQueryParser::Act_for_attributesContext *context) = 0;

    virtual std::any visitNo_cream_off(KnoBABQueryParser::No_cream_offContext *context) = 0;

    virtual std::any visitWith_data(KnoBABQueryParser::With_dataContext *context) = 0;

    virtual std::any visitWith_missing(KnoBABQueryParser::With_missingContext *context) = 0;

    virtual std::any visitNo_stats(KnoBABQueryParser::No_statsContext *context) = 0;

    virtual std::any visitVar(KnoBABQueryParser::VarContext *context) = 0;


};

