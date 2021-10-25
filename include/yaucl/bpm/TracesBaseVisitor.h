
// Generated from src/bpm/Traces.g by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "TracesVisitor.h"


/**
 * This class provides an empty implementation of TracesVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  TracesBaseVisitor : public TracesVisitor {
public:

  virtual antlrcpp::Any visitLog(TracesParser::LogContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTrace(TracesParser::TraceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEvent(TracesParser::EventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitData_part(TracesParser::Data_partContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitField(TracesParser::FieldContext *ctx) override {
    return visitChildren(ctx);
  }


};

