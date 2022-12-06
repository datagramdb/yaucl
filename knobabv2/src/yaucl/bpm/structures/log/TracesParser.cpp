
// Generated from src/yaucl/bpm/structures/log/Traces.g4 by ANTLR 4.10.1


#include "yaucl/bpm/structures/log/TracesListener.h"
#include "yaucl/bpm/structures/log/TracesVisitor.h"

#include "yaucl/bpm/structures/log/TracesParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct TracesParserStaticData final {
  TracesParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  TracesParserStaticData(const TracesParserStaticData&) = delete;
  TracesParserStaticData(TracesParserStaticData&&) = delete;
  TracesParserStaticData& operator=(const TracesParserStaticData&) = delete;
  TracesParserStaticData& operator=(TracesParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

std::once_flag tracesParserOnceFlag;
TracesParserStaticData *tracesParserStaticData = nullptr;

void tracesParserInitialize() {
  assert(tracesParserStaticData == nullptr);
  auto staticData = std::make_unique<TracesParserStaticData>(
    std::vector<std::string>{
      "log", "trace", "event", "data_part", "field"
    },
    std::vector<std::string>{
      "", "';'", "':'", "'{'", "'}'", "'='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "VAR", "LABEL", "INTNUMBER", "NUMBER", "STRING", 
      "SPACE", "COMMENT", "LINE_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,13,46,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,1,0,1,0,1,0,5,0,14,
  	8,0,10,0,12,0,17,9,0,1,0,1,0,1,1,1,1,1,1,5,1,24,8,1,10,1,12,1,27,9,1,
  	1,2,1,2,3,2,31,8,2,1,3,1,3,5,3,35,8,3,10,3,12,3,38,9,3,1,3,1,3,1,4,1,
  	4,1,4,1,4,1,4,0,0,5,0,2,4,6,8,0,1,1,0,9,10,44,0,15,1,0,0,0,2,20,1,0,0,
  	0,4,28,1,0,0,0,6,32,1,0,0,0,8,41,1,0,0,0,10,11,3,2,1,0,11,12,5,1,0,0,
  	12,14,1,0,0,0,13,10,1,0,0,0,14,17,1,0,0,0,15,13,1,0,0,0,15,16,1,0,0,0,
  	16,18,1,0,0,0,17,15,1,0,0,0,18,19,3,2,1,0,19,1,1,0,0,0,20,21,3,6,3,0,
  	21,25,5,2,0,0,22,24,3,4,2,0,23,22,1,0,0,0,24,27,1,0,0,0,25,23,1,0,0,0,
  	25,26,1,0,0,0,26,3,1,0,0,0,27,25,1,0,0,0,28,30,5,7,0,0,29,31,3,6,3,0,
  	30,29,1,0,0,0,30,31,1,0,0,0,31,5,1,0,0,0,32,36,5,3,0,0,33,35,3,8,4,0,
  	34,33,1,0,0,0,35,38,1,0,0,0,36,34,1,0,0,0,36,37,1,0,0,0,37,39,1,0,0,0,
  	38,36,1,0,0,0,39,40,5,4,0,0,40,7,1,0,0,0,41,42,5,6,0,0,42,43,5,5,0,0,
  	43,44,7,0,0,0,44,9,1,0,0,0,4,15,25,30,36
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  tracesParserStaticData = staticData.release();
}

}

TracesParser::TracesParser(TokenStream *input) : TracesParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

TracesParser::TracesParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  TracesParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *tracesParserStaticData->atn, tracesParserStaticData->decisionToDFA, tracesParserStaticData->sharedContextCache, options);
}

TracesParser::~TracesParser() {
  delete _interpreter;
}

const atn::ATN& TracesParser::getATN() const {
  return *tracesParserStaticData->atn;
}

std::string TracesParser::getGrammarFileName() const {
  return "Traces.g4";
}

const std::vector<std::string>& TracesParser::getRuleNames() const {
  return tracesParserStaticData->ruleNames;
}

const dfa::Vocabulary& TracesParser::getVocabulary() const {
  return tracesParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView TracesParser::getSerializedATN() const {
  return tracesParserStaticData->serializedATN;
}


//----------------- LogContext ------------------------------------------------------------------

TracesParser::LogContext::LogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<TracesParser::TraceContext *> TracesParser::LogContext::trace() {
  return getRuleContexts<TracesParser::TraceContext>();
}

TracesParser::TraceContext* TracesParser::LogContext::trace(size_t i) {
  return getRuleContext<TracesParser::TraceContext>(i);
}


size_t TracesParser::LogContext::getRuleIndex() const {
  return TracesParser::RuleLog;
}

void TracesParser::LogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLog(this);
}

void TracesParser::LogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLog(this);
}


std::any TracesParser::LogContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TracesVisitor*>(visitor))
    return parserVisitor->visitLog(this);
  else
    return visitor->visitChildren(this);
}

TracesParser::LogContext* TracesParser::log() {
  LogContext *_localctx = _tracker.createInstance<LogContext>(_ctx, getState());
  enterRule(_localctx, 0, TracesParser::RuleLog);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(15);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(10);
        trace();
        setState(11);
        match(TracesParser::T__0); 
      }
      setState(17);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    }
    setState(18);
    trace();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TraceContext ------------------------------------------------------------------

TracesParser::TraceContext::TraceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

TracesParser::Data_partContext* TracesParser::TraceContext::data_part() {
  return getRuleContext<TracesParser::Data_partContext>(0);
}

std::vector<TracesParser::EventContext *> TracesParser::TraceContext::event() {
  return getRuleContexts<TracesParser::EventContext>();
}

TracesParser::EventContext* TracesParser::TraceContext::event(size_t i) {
  return getRuleContext<TracesParser::EventContext>(i);
}


size_t TracesParser::TraceContext::getRuleIndex() const {
  return TracesParser::RuleTrace;
}

void TracesParser::TraceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTrace(this);
}

void TracesParser::TraceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTrace(this);
}


std::any TracesParser::TraceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TracesVisitor*>(visitor))
    return parserVisitor->visitTrace(this);
  else
    return visitor->visitChildren(this);
}

TracesParser::TraceContext* TracesParser::trace() {
  TraceContext *_localctx = _tracker.createInstance<TraceContext>(_ctx, getState());
  enterRule(_localctx, 2, TracesParser::RuleTrace);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(20);
    data_part();
    setState(21);
    match(TracesParser::T__1);
    setState(25);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == TracesParser::LABEL) {
      setState(22);
      event();
      setState(27);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EventContext ------------------------------------------------------------------

TracesParser::EventContext::EventContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* TracesParser::EventContext::LABEL() {
  return getToken(TracesParser::LABEL, 0);
}

TracesParser::Data_partContext* TracesParser::EventContext::data_part() {
  return getRuleContext<TracesParser::Data_partContext>(0);
}


size_t TracesParser::EventContext::getRuleIndex() const {
  return TracesParser::RuleEvent;
}

void TracesParser::EventContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEvent(this);
}

void TracesParser::EventContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEvent(this);
}


std::any TracesParser::EventContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TracesVisitor*>(visitor))
    return parserVisitor->visitEvent(this);
  else
    return visitor->visitChildren(this);
}

TracesParser::EventContext* TracesParser::event() {
  EventContext *_localctx = _tracker.createInstance<EventContext>(_ctx, getState());
  enterRule(_localctx, 4, TracesParser::RuleEvent);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    match(TracesParser::LABEL);
    setState(30);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == TracesParser::T__2) {
      setState(29);
      data_part();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Data_partContext ------------------------------------------------------------------

TracesParser::Data_partContext::Data_partContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<TracesParser::FieldContext *> TracesParser::Data_partContext::field() {
  return getRuleContexts<TracesParser::FieldContext>();
}

TracesParser::FieldContext* TracesParser::Data_partContext::field(size_t i) {
  return getRuleContext<TracesParser::FieldContext>(i);
}


size_t TracesParser::Data_partContext::getRuleIndex() const {
  return TracesParser::RuleData_part;
}

void TracesParser::Data_partContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterData_part(this);
}

void TracesParser::Data_partContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitData_part(this);
}


std::any TracesParser::Data_partContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TracesVisitor*>(visitor))
    return parserVisitor->visitData_part(this);
  else
    return visitor->visitChildren(this);
}

TracesParser::Data_partContext* TracesParser::data_part() {
  Data_partContext *_localctx = _tracker.createInstance<Data_partContext>(_ctx, getState());
  enterRule(_localctx, 6, TracesParser::RuleData_part);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(32);
    match(TracesParser::T__2);
    setState(36);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == TracesParser::VAR) {
      setState(33);
      field();
      setState(38);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(39);
    match(TracesParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FieldContext ------------------------------------------------------------------

TracesParser::FieldContext::FieldContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* TracesParser::FieldContext::VAR() {
  return getToken(TracesParser::VAR, 0);
}

tree::TerminalNode* TracesParser::FieldContext::NUMBER() {
  return getToken(TracesParser::NUMBER, 0);
}

tree::TerminalNode* TracesParser::FieldContext::STRING() {
  return getToken(TracesParser::STRING, 0);
}


size_t TracesParser::FieldContext::getRuleIndex() const {
  return TracesParser::RuleField;
}

void TracesParser::FieldContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterField(this);
}

void TracesParser::FieldContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<TracesListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitField(this);
}


std::any TracesParser::FieldContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<TracesVisitor*>(visitor))
    return parserVisitor->visitField(this);
  else
    return visitor->visitChildren(this);
}

TracesParser::FieldContext* TracesParser::field() {
  FieldContext *_localctx = _tracker.createInstance<FieldContext>(_ctx, getState());
  enterRule(_localctx, 8, TracesParser::RuleField);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(41);
    match(TracesParser::VAR);
    setState(42);
    match(TracesParser::T__4);
    setState(43);
    _la = _input->LA(1);
    if (!(_la == TracesParser::NUMBER

    || _la == TracesParser::STRING)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void TracesParser::initialize() {
  std::call_once(tracesParserOnceFlag, tracesParserInitialize);
}
