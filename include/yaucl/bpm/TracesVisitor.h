
// Generated from src/bpm/Traces.g by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "TracesParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by TracesParser.
 */
class  TracesVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by TracesParser.
   */
    virtual antlrcpp::Any visitLog(TracesParser::LogContext *context) = 0;

    virtual antlrcpp::Any visitTrace(TracesParser::TraceContext *context) = 0;

    virtual antlrcpp::Any visitEvent(TracesParser::EventContext *context) = 0;

    virtual antlrcpp::Any visitData_part(TracesParser::Data_partContext *context) = 0;

    virtual antlrcpp::Any visitField(TracesParser::FieldContext *context) = 0;


};

