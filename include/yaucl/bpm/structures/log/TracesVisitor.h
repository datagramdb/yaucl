
// Generated from src/yaucl/bpm/structures/log/Traces.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "yaucl/bpm/structures/log/TracesParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by TracesParser.
 */
class  TracesVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by TracesParser.
   */
    virtual std::any visitLog(TracesParser::LogContext *context) = 0;

    virtual std::any visitTrace(TracesParser::TraceContext *context) = 0;

    virtual std::any visitEvent(TracesParser::EventContext *context) = 0;

    virtual std::any visitData_part(TracesParser::Data_partContext *context) = 0;

    virtual std::any visitField(TracesParser::FieldContext *context) = 0;


};

