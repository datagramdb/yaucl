
// Generated from src/bpm/Traces.g by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "TracesListener.h"


/**
 * This class provides an empty implementation of TracesListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  TracesBaseListener : public TracesListener {
public:

  virtual void enterLog(TracesParser::LogContext * /*ctx*/) override { }
  virtual void exitLog(TracesParser::LogContext * /*ctx*/) override { }

  virtual void enterTrace(TracesParser::TraceContext * /*ctx*/) override { }
  virtual void exitTrace(TracesParser::TraceContext * /*ctx*/) override { }

  virtual void enterEvent(TracesParser::EventContext * /*ctx*/) override { }
  virtual void exitEvent(TracesParser::EventContext * /*ctx*/) override { }

  virtual void enterData_part(TracesParser::Data_partContext * /*ctx*/) override { }
  virtual void exitData_part(TracesParser::Data_partContext * /*ctx*/) override { }

  virtual void enterField(TracesParser::FieldContext * /*ctx*/) override { }
  virtual void exitField(TracesParser::FieldContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

