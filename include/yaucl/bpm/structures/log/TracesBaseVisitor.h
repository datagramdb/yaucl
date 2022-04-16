
// Generated from src/yaucl/bpm/structures/log/Traces.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "yaucl/bpm/structures/log/TracesVisitor.h"


/**
 * This class provides an empty implementation of TracesVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  TracesBaseVisitor : public TracesVisitor {
public:

  virtual std::any visitLog(TracesParser::LogContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrace(TracesParser::TraceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent(TracesParser::EventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_part(TracesParser::Data_partContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitField(TracesParser::FieldContext *ctx) override {
    return visitChildren(ctx);
  }


};

