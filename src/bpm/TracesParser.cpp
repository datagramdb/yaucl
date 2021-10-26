
// Generated from src/bpm/Traces.g by ANTLR 4.9.2


#include "TracesListener.h"
#include "TracesVisitor.h"

#include "TracesParser.h"


using namespace antlrcpp;
using namespace antlr4;

TracesParser::TracesParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

TracesParser::~TracesParser() {
  delete _interpreter;
}

std::string TracesParser::getGrammarFileName() const {
  return "Traces.g";
}

const std::vector<std::string>& TracesParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& TracesParser::getVocabulary() const {
  return _vocabulary;
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


antlrcpp::Any TracesParser::LogContext::accept(tree::ParseTreeVisitor *visitor) {
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


antlrcpp::Any TracesParser::TraceContext::accept(tree::ParseTreeVisitor *visitor) {
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


antlrcpp::Any TracesParser::EventContext::accept(tree::ParseTreeVisitor *visitor) {
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


antlrcpp::Any TracesParser::Data_partContext::accept(tree::ParseTreeVisitor *visitor) {
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


antlrcpp::Any TracesParser::FieldContext::accept(tree::ParseTreeVisitor *visitor) {
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

// Static vars and initialization.
std::vector<dfa::DFA> TracesParser::_decisionToDFA;
atn::PredictionContextCache TracesParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN TracesParser::_atn;
std::vector<uint16_t> TracesParser::_serializedATN;

std::vector<std::string> TracesParser::_ruleNames = {
  "log", "trace", "event", "data_part", "field"
};

std::vector<std::string> TracesParser::_literalNames = {
  "", "';'", "':'", "'{'", "'}'", "'='"
};

std::vector<std::string> TracesParser::_symbolicNames = {
  "", "", "", "", "", "", "VAR", "LABEL", "INTNUMBER", "NUMBER", "STRING", 
  "SPACE", "COMMENT", "LINE_COMMENT"
};

dfa::Vocabulary TracesParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> TracesParser::_tokenNames;

TracesParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  static const uint16_t serializedATNSegment0[] = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
       0x3, 0xf, 0x30, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
       0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x3, 0x2, 0x3, 
       0x2, 0x3, 0x2, 0x7, 0x2, 0x10, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x13, 
       0xb, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 
       0x3, 0x1a, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x1d, 0xb, 0x3, 0x3, 0x4, 
       0x3, 0x4, 0x5, 0x4, 0x21, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x7, 0x5, 
       0x25, 0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x28, 0xb, 0x5, 0x3, 0x5, 0x3, 
       0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x2, 0x2, 
       0x7, 0x2, 0x4, 0x6, 0x8, 0xa, 0x2, 0x3, 0x3, 0x2, 0xb, 0xc, 0x2, 
       0x2e, 0x2, 0x11, 0x3, 0x2, 0x2, 0x2, 0x4, 0x16, 0x3, 0x2, 0x2, 0x2, 
       0x6, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x8, 0x22, 0x3, 0x2, 0x2, 0x2, 0xa, 
       0x2b, 0x3, 0x2, 0x2, 0x2, 0xc, 0xd, 0x5, 0x4, 0x3, 0x2, 0xd, 0xe, 
       0x7, 0x3, 0x2, 0x2, 0xe, 0x10, 0x3, 0x2, 0x2, 0x2, 0xf, 0xc, 0x3, 
       0x2, 0x2, 0x2, 0x10, 0x13, 0x3, 0x2, 0x2, 0x2, 0x11, 0xf, 0x3, 0x2, 
       0x2, 0x2, 0x11, 0x12, 0x3, 0x2, 0x2, 0x2, 0x12, 0x14, 0x3, 0x2, 0x2, 
       0x2, 0x13, 0x11, 0x3, 0x2, 0x2, 0x2, 0x14, 0x15, 0x5, 0x4, 0x3, 0x2, 
       0x15, 0x3, 0x3, 0x2, 0x2, 0x2, 0x16, 0x17, 0x5, 0x8, 0x5, 0x2, 0x17, 
       0x1b, 0x7, 0x4, 0x2, 0x2, 0x18, 0x1a, 0x5, 0x6, 0x4, 0x2, 0x19, 0x18, 
       0x3, 0x2, 0x2, 0x2, 0x1a, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x19, 0x3, 
       0x2, 0x2, 0x2, 0x1b, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x5, 0x3, 0x2, 
       0x2, 0x2, 0x1d, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x20, 0x7, 0x9, 0x2, 
       0x2, 0x1f, 0x21, 0x5, 0x8, 0x5, 0x2, 0x20, 0x1f, 0x3, 0x2, 0x2, 0x2, 
       0x20, 0x21, 0x3, 0x2, 0x2, 0x2, 0x21, 0x7, 0x3, 0x2, 0x2, 0x2, 0x22, 
       0x26, 0x7, 0x5, 0x2, 0x2, 0x23, 0x25, 0x5, 0xa, 0x6, 0x2, 0x24, 0x23, 
       0x3, 0x2, 0x2, 0x2, 0x25, 0x28, 0x3, 0x2, 0x2, 0x2, 0x26, 0x24, 0x3, 
       0x2, 0x2, 0x2, 0x26, 0x27, 0x3, 0x2, 0x2, 0x2, 0x27, 0x29, 0x3, 0x2, 
       0x2, 0x2, 0x28, 0x26, 0x3, 0x2, 0x2, 0x2, 0x29, 0x2a, 0x7, 0x6, 0x2, 
       0x2, 0x2a, 0x9, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2c, 0x7, 0x8, 0x2, 0x2, 
       0x2c, 0x2d, 0x7, 0x7, 0x2, 0x2, 0x2d, 0x2e, 0x9, 0x2, 0x2, 0x2, 0x2e, 
       0xb, 0x3, 0x2, 0x2, 0x2, 0x6, 0x11, 0x1b, 0x20, 0x26, 
  };

  _serializedATN.insert(_serializedATN.end(), serializedATNSegment0,
    serializedATNSegment0 + sizeof(serializedATNSegment0) / sizeof(serializedATNSegment0[0]));


  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

TracesParser::Initializer TracesParser::_init;
