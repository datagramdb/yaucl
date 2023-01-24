
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1


#include "yaucl/graphs/flloat/FLLOATPropListener.h"
#include "yaucl/graphs/flloat/FLLOATPropVisitor.h"

#include "yaucl/graphs/flloat/FLLOATPropParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct FLLOATPropParserStaticData final {
  FLLOATPropParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FLLOATPropParserStaticData(const FLLOATPropParserStaticData&) = delete;
  FLLOATPropParserStaticData(FLLOATPropParserStaticData&&) = delete;
  FLLOATPropParserStaticData& operator=(const FLLOATPropParserStaticData&) = delete;
  FLLOATPropParserStaticData& operator=(FLLOATPropParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag flloatpropParserOnceFlag;
FLLOATPropParserStaticData *flloatpropParserStaticData = nullptr;

void flloatpropParserInitialize() {
  assert(flloatpropParserStaticData == nullptr);
  auto staticData = std::make_unique<FLLOATPropParserStaticData>(
    std::vector<std::string>{
      "statement"
    },
    std::vector<std::string>{
      "", "'~'", "'('", "')'", "'&'", "'|'", "'True'", "'False'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "NUMBER", "ID", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,10,26,2,0,7,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,0,13,8,0,
  	1,0,1,0,1,0,1,0,1,0,1,0,5,0,21,8,0,10,0,12,0,24,9,0,1,0,0,1,0,1,0,0,0,
  	30,0,12,1,0,0,0,2,3,6,0,-1,0,3,4,5,1,0,0,4,13,3,0,0,7,5,13,5,9,0,0,6,
  	7,5,2,0,0,7,8,3,0,0,0,8,9,5,3,0,0,9,13,1,0,0,0,10,13,5,6,0,0,11,13,5,
  	7,0,0,12,2,1,0,0,0,12,5,1,0,0,0,12,6,1,0,0,0,12,10,1,0,0,0,12,11,1,0,
  	0,0,13,22,1,0,0,0,14,15,10,4,0,0,15,16,5,4,0,0,16,21,3,0,0,4,17,18,10,
  	3,0,0,18,19,5,5,0,0,19,21,3,0,0,3,20,14,1,0,0,0,20,17,1,0,0,0,21,24,1,
  	0,0,0,22,20,1,0,0,0,22,23,1,0,0,0,23,1,1,0,0,0,24,22,1,0,0,0,3,12,20,
  	22
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  flloatpropParserStaticData = staticData.release();
}

}

FLLOATPropParser::FLLOATPropParser(TokenStream *input) : FLLOATPropParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

FLLOATPropParser::FLLOATPropParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  FLLOATPropParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *flloatpropParserStaticData->atn, flloatpropParserStaticData->decisionToDFA, flloatpropParserStaticData->sharedContextCache, options);
}

FLLOATPropParser::~FLLOATPropParser() {
  delete _interpreter;
}

const atn::ATN& FLLOATPropParser::getATN() const {
  return *flloatpropParserStaticData->atn;
}

std::string FLLOATPropParser::getGrammarFileName() const {
  return "FLLOATProp.g4";
}

const std::vector<std::string>& FLLOATPropParser::getRuleNames() const {
  return flloatpropParserStaticData->ruleNames;
}

const dfa::Vocabulary& FLLOATPropParser::getVocabulary() const {
  return flloatpropParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView FLLOATPropParser::getSerializedATN() const {
  return flloatpropParserStaticData->serializedATN;
}


//----------------- StatementContext ------------------------------------------------------------------

FLLOATPropParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t FLLOATPropParser::StatementContext::getRuleIndex() const {
  return FLLOATPropParser::RuleStatement;
}

void FLLOATPropParser::StatementContext::copyFrom(StatementContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- NegationContext ------------------------------------------------------------------

FLLOATPropParser::StatementContext* FLLOATPropParser::NegationContext::statement() {
  return getRuleContext<FLLOATPropParser::StatementContext>(0);
}

FLLOATPropParser::NegationContext::NegationContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::NegationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNegation(this);
}
void FLLOATPropParser::NegationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNegation(this);
}

std::any FLLOATPropParser::NegationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitNegation(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ParenContext ------------------------------------------------------------------

FLLOATPropParser::StatementContext* FLLOATPropParser::ParenContext::statement() {
  return getRuleContext<FLLOATPropParser::StatementContext>(0);
}

FLLOATPropParser::ParenContext::ParenContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::ParenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParen(this);
}
void FLLOATPropParser::ParenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParen(this);
}

std::any FLLOATPropParser::ParenContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitParen(this);
  else
    return visitor->visitChildren(this);
}
//----------------- OrContext ------------------------------------------------------------------

std::vector<FLLOATPropParser::StatementContext *> FLLOATPropParser::OrContext::statement() {
  return getRuleContexts<FLLOATPropParser::StatementContext>();
}

FLLOATPropParser::StatementContext* FLLOATPropParser::OrContext::statement(size_t i) {
  return getRuleContext<FLLOATPropParser::StatementContext>(i);
}

FLLOATPropParser::OrContext::OrContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::OrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOr(this);
}
void FLLOATPropParser::OrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOr(this);
}

std::any FLLOATPropParser::OrContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitOr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TopContext ------------------------------------------------------------------

FLLOATPropParser::TopContext::TopContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::TopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTop(this);
}
void FLLOATPropParser::TopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTop(this);
}

std::any FLLOATPropParser::TopContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitTop(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BotContext ------------------------------------------------------------------

FLLOATPropParser::BotContext::BotContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::BotContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBot(this);
}
void FLLOATPropParser::BotContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBot(this);
}

std::any FLLOATPropParser::BotContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitBot(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AndContext ------------------------------------------------------------------

std::vector<FLLOATPropParser::StatementContext *> FLLOATPropParser::AndContext::statement() {
  return getRuleContexts<FLLOATPropParser::StatementContext>();
}

FLLOATPropParser::StatementContext* FLLOATPropParser::AndContext::statement(size_t i) {
  return getRuleContext<FLLOATPropParser::StatementContext>(i);
}

FLLOATPropParser::AndContext::AndContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::AndContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnd(this);
}
void FLLOATPropParser::AndContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnd(this);
}

std::any FLLOATPropParser::AndContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitAnd(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AtomContext ------------------------------------------------------------------

tree::TerminalNode* FLLOATPropParser::AtomContext::ID() {
  return getToken(FLLOATPropParser::ID, 0);
}

FLLOATPropParser::AtomContext::AtomContext(StatementContext *ctx) { copyFrom(ctx); }

void FLLOATPropParser::AtomContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtom(this);
}
void FLLOATPropParser::AtomContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FLLOATPropListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtom(this);
}

std::any FLLOATPropParser::AtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<FLLOATPropVisitor*>(visitor))
    return parserVisitor->visitAtom(this);
  else
    return visitor->visitChildren(this);
}

FLLOATPropParser::StatementContext* FLLOATPropParser::statement() {
   return statement(0);
}

FLLOATPropParser::StatementContext* FLLOATPropParser::statement(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  FLLOATPropParser::StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, parentState);
  FLLOATPropParser::StatementContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 0;
  enterRecursionRule(_localctx, 0, FLLOATPropParser::RuleStatement, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(12);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FLLOATPropParser::T__0: {
        _localctx = _tracker.createInstance<NegationContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(3);
        match(FLLOATPropParser::T__0);
        setState(4);
        statement(7);
        break;
      }

      case FLLOATPropParser::ID: {
        _localctx = _tracker.createInstance<AtomContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(5);
        match(FLLOATPropParser::ID);
        break;
      }

      case FLLOATPropParser::T__1: {
        _localctx = _tracker.createInstance<ParenContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(6);
        match(FLLOATPropParser::T__1);
        setState(7);
        statement(0);
        setState(8);
        match(FLLOATPropParser::T__2);
        break;
      }

      case FLLOATPropParser::T__5: {
        _localctx = _tracker.createInstance<TopContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(10);
        match(FLLOATPropParser::T__5);
        break;
      }

      case FLLOATPropParser::T__6: {
        _localctx = _tracker.createInstance<BotContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(11);
        match(FLLOATPropParser::T__6);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(22);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(20);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<AndContext>(_tracker.createInstance<StatementContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleStatement);
          setState(14);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(15);
          match(FLLOATPropParser::T__3);
          setState(16);
          statement(4);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<OrContext>(_tracker.createInstance<StatementContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleStatement);
          setState(17);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(18);
          match(FLLOATPropParser::T__4);
          setState(19);
          statement(3);
          break;
        }

        default:
          break;
        } 
      }
      setState(24);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

bool FLLOATPropParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 0: return statementSempred(antlrcpp::downCast<StatementContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool FLLOATPropParser::statementSempred(StatementContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 4);
    case 1: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

void FLLOATPropParser::initialize() {
  ::antlr4::internal::call_once(flloatpropParserOnceFlag, flloatpropParserInitialize);
}
