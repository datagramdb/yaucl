
// Generated from src/yaucl/bpm/structures/log/Traces.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "yaucl/bpm/structures/log/TracesParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by TracesParser.
 */
class  TracesListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterLog(TracesParser::LogContext *ctx) = 0;
  virtual void exitLog(TracesParser::LogContext *ctx) = 0;

  virtual void enterTrace(TracesParser::TraceContext *ctx) = 0;
  virtual void exitTrace(TracesParser::TraceContext *ctx) = 0;

  virtual void enterEvent(TracesParser::EventContext *ctx) = 0;
  virtual void exitEvent(TracesParser::EventContext *ctx) = 0;

  virtual void enterData_part(TracesParser::Data_partContext *ctx) = 0;
  virtual void exitData_part(TracesParser::Data_partContext *ctx) = 0;

  virtual void enterField(TracesParser::FieldContext *ctx) = 0;
  virtual void exitField(TracesParser::FieldContext *ctx) = 0;


};

