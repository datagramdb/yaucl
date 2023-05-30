
// Generated from include/yaucl/graphs/DOT.g4 by ANTLR 4.11.1


#include "yaucl/graphs/dot/DOTListener.h"
#include "yaucl/graphs/dot/DOTVisitor.h"

#include "yaucl/graphs/dot/DOTParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct DOTParserStaticData final {
  DOTParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  DOTParserStaticData(const DOTParserStaticData&) = delete;
  DOTParserStaticData(DOTParserStaticData&&) = delete;
  DOTParserStaticData& operator=(const DOTParserStaticData&) = delete;
  DOTParserStaticData& operator=(DOTParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag dotParserOnceFlag;
DOTParserStaticData *dotParserStaticData = nullptr;

void dotParserInitialize() {
  assert(dotParserStaticData == nullptr);
  auto staticData = std::make_unique<DOTParserStaticData>(
    std::vector<std::string>{
      "graph", "stmt_list", "stmt", "attr_stmt", "attr_list", "a_list", 
      "edge_stmt", "edgeRHS", "edgeop", "node_stmt", "node_id", "port", 
      "subgraph", "id"
    },
    std::vector<std::string>{
      "", "'{'", "'}'", "';'", "'='", "'['", "']'", "','", "'->'", "'--'", 
      "':'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "STRICT", "GRAPH", "DIGRAPH", 
      "NODE", "EDGE", "SUBGRAPH", "NUMBER", "STRING", "ID", "HTML_STRING", 
      "COMMENT", "LINE_COMMENT", "PREPROC", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,24,128,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,1,0,3,0,
  	30,8,0,1,0,1,0,3,0,34,8,0,1,0,1,0,1,0,1,0,1,1,1,1,3,1,42,8,1,5,1,44,8,
  	1,10,1,12,1,47,9,1,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2,57,8,2,1,3,1,3,
  	1,3,1,4,1,4,3,4,64,8,4,1,4,4,4,67,8,4,11,4,12,4,68,1,5,1,5,1,5,3,5,74,
  	8,5,1,5,3,5,77,8,5,4,5,79,8,5,11,5,12,5,80,1,6,1,6,3,6,85,8,6,1,6,1,6,
  	3,6,89,8,6,1,7,1,7,1,7,3,7,94,8,7,4,7,96,8,7,11,7,12,7,97,1,8,1,8,1,9,
  	1,9,3,9,104,8,9,1,10,1,10,3,10,108,8,10,1,11,1,11,1,11,1,11,3,11,114,
  	8,11,1,12,1,12,3,12,118,8,12,3,12,120,8,12,1,12,1,12,1,12,1,12,1,13,1,
  	13,1,13,0,0,14,0,2,4,6,8,10,12,14,16,18,20,22,24,26,0,4,1,0,12,13,2,0,
  	12,12,14,15,1,0,8,9,1,0,17,20,135,0,29,1,0,0,0,2,45,1,0,0,0,4,56,1,0,
  	0,0,6,58,1,0,0,0,8,66,1,0,0,0,10,78,1,0,0,0,12,84,1,0,0,0,14,95,1,0,0,
  	0,16,99,1,0,0,0,18,101,1,0,0,0,20,105,1,0,0,0,22,109,1,0,0,0,24,119,1,
  	0,0,0,26,125,1,0,0,0,28,30,5,11,0,0,29,28,1,0,0,0,29,30,1,0,0,0,30,31,
  	1,0,0,0,31,33,7,0,0,0,32,34,3,26,13,0,33,32,1,0,0,0,33,34,1,0,0,0,34,
  	35,1,0,0,0,35,36,5,1,0,0,36,37,3,2,1,0,37,38,5,2,0,0,38,1,1,0,0,0,39,
  	41,3,4,2,0,40,42,5,3,0,0,41,40,1,0,0,0,41,42,1,0,0,0,42,44,1,0,0,0,43,
  	39,1,0,0,0,44,47,1,0,0,0,45,43,1,0,0,0,45,46,1,0,0,0,46,3,1,0,0,0,47,
  	45,1,0,0,0,48,57,3,18,9,0,49,57,3,12,6,0,50,57,3,6,3,0,51,52,3,26,13,
  	0,52,53,5,4,0,0,53,54,3,26,13,0,54,57,1,0,0,0,55,57,3,24,12,0,56,48,1,
  	0,0,0,56,49,1,0,0,0,56,50,1,0,0,0,56,51,1,0,0,0,56,55,1,0,0,0,57,5,1,
  	0,0,0,58,59,7,1,0,0,59,60,3,8,4,0,60,7,1,0,0,0,61,63,5,5,0,0,62,64,3,
  	10,5,0,63,62,1,0,0,0,63,64,1,0,0,0,64,65,1,0,0,0,65,67,5,6,0,0,66,61,
  	1,0,0,0,67,68,1,0,0,0,68,66,1,0,0,0,68,69,1,0,0,0,69,9,1,0,0,0,70,73,
  	3,26,13,0,71,72,5,4,0,0,72,74,3,26,13,0,73,71,1,0,0,0,73,74,1,0,0,0,74,
  	76,1,0,0,0,75,77,5,7,0,0,76,75,1,0,0,0,76,77,1,0,0,0,77,79,1,0,0,0,78,
  	70,1,0,0,0,79,80,1,0,0,0,80,78,1,0,0,0,80,81,1,0,0,0,81,11,1,0,0,0,82,
  	85,3,20,10,0,83,85,3,24,12,0,84,82,1,0,0,0,84,83,1,0,0,0,85,86,1,0,0,
  	0,86,88,3,14,7,0,87,89,3,8,4,0,88,87,1,0,0,0,88,89,1,0,0,0,89,13,1,0,
  	0,0,90,93,3,16,8,0,91,94,3,20,10,0,92,94,3,24,12,0,93,91,1,0,0,0,93,92,
  	1,0,0,0,94,96,1,0,0,0,95,90,1,0,0,0,96,97,1,0,0,0,97,95,1,0,0,0,97,98,
  	1,0,0,0,98,15,1,0,0,0,99,100,7,2,0,0,100,17,1,0,0,0,101,103,3,20,10,0,
  	102,104,3,8,4,0,103,102,1,0,0,0,103,104,1,0,0,0,104,19,1,0,0,0,105,107,
  	3,26,13,0,106,108,3,22,11,0,107,106,1,0,0,0,107,108,1,0,0,0,108,21,1,
  	0,0,0,109,110,5,10,0,0,110,113,3,26,13,0,111,112,5,10,0,0,112,114,3,26,
  	13,0,113,111,1,0,0,0,113,114,1,0,0,0,114,23,1,0,0,0,115,117,5,16,0,0,
  	116,118,3,26,13,0,117,116,1,0,0,0,117,118,1,0,0,0,118,120,1,0,0,0,119,
  	115,1,0,0,0,119,120,1,0,0,0,120,121,1,0,0,0,121,122,5,1,0,0,122,123,3,
  	2,1,0,123,124,5,2,0,0,124,25,1,0,0,0,125,126,7,3,0,0,126,27,1,0,0,0,19,
  	29,33,41,45,56,63,68,73,76,80,84,88,93,97,103,107,113,117,119
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  dotParserStaticData = staticData.release();
}

}

DOTParser::DOTParser(TokenStream *input) : DOTParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

DOTParser::DOTParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  DOTParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *dotParserStaticData->atn, dotParserStaticData->decisionToDFA, dotParserStaticData->sharedContextCache, options);
}

DOTParser::~DOTParser() {
  delete _interpreter;
}

const atn::ATN& DOTParser::getATN() const {
  return *dotParserStaticData->atn;
}

std::string DOTParser::getGrammarFileName() const {
  return "DOT.g4";
}

const std::vector<std::string>& DOTParser::getRuleNames() const {
  return dotParserStaticData->ruleNames;
}

const dfa::Vocabulary& DOTParser::getVocabulary() const {
  return dotParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView DOTParser::getSerializedATN() const {
  return dotParserStaticData->serializedATN;
}


//----------------- GraphContext ------------------------------------------------------------------

DOTParser::GraphContext::GraphContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::Stmt_listContext* DOTParser::GraphContext::stmt_list() {
  return getRuleContext<DOTParser::Stmt_listContext>(0);
}

tree::TerminalNode* DOTParser::GraphContext::GRAPH() {
  return getToken(DOTParser::GRAPH, 0);
}

tree::TerminalNode* DOTParser::GraphContext::DIGRAPH() {
  return getToken(DOTParser::DIGRAPH, 0);
}

tree::TerminalNode* DOTParser::GraphContext::STRICT() {
  return getToken(DOTParser::STRICT, 0);
}

DOTParser::IdContext* DOTParser::GraphContext::id() {
  return getRuleContext<DOTParser::IdContext>(0);
}


size_t DOTParser::GraphContext::getRuleIndex() const {
  return DOTParser::RuleGraph;
}

void DOTParser::GraphContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGraph(this);
}

void DOTParser::GraphContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGraph(this);
}


std::any DOTParser::GraphContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitGraph(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::GraphContext* DOTParser::graph() {
  GraphContext *_localctx = _tracker.createInstance<GraphContext>(_ctx, getState());
  enterRule(_localctx, 0, DOTParser::RuleGraph);
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
    setState(29);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::STRICT) {
      setState(28);
      match(DOTParser::STRICT);
    }
    setState(31);
    _la = _input->LA(1);
    if (!(_la == DOTParser::GRAPH

    || _la == DOTParser::DIGRAPH)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(33);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1966080) != 0) {
      setState(32);
      id();
    }
    setState(35);
    match(DOTParser::T__0);
    setState(36);
    stmt_list();
    setState(37);
    match(DOTParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Stmt_listContext ------------------------------------------------------------------

DOTParser::Stmt_listContext::Stmt_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DOTParser::StmtContext *> DOTParser::Stmt_listContext::stmt() {
  return getRuleContexts<DOTParser::StmtContext>();
}

DOTParser::StmtContext* DOTParser::Stmt_listContext::stmt(size_t i) {
  return getRuleContext<DOTParser::StmtContext>(i);
}


size_t DOTParser::Stmt_listContext::getRuleIndex() const {
  return DOTParser::RuleStmt_list;
}

void DOTParser::Stmt_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt_list(this);
}

void DOTParser::Stmt_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt_list(this);
}


std::any DOTParser::Stmt_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitStmt_list(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Stmt_listContext* DOTParser::stmt_list() {
  Stmt_listContext *_localctx = _tracker.createInstance<Stmt_listContext>(_ctx, getState());
  enterRule(_localctx, 2, DOTParser::RuleStmt_list);
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
    setState(45);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2084866) != 0) {
      setState(39);
      stmt();
      setState(41);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == DOTParser::T__2) {
        setState(40);
        match(DOTParser::T__2);
      }
      setState(47);
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

//----------------- StmtContext ------------------------------------------------------------------

DOTParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::Node_stmtContext* DOTParser::StmtContext::node_stmt() {
  return getRuleContext<DOTParser::Node_stmtContext>(0);
}

DOTParser::Edge_stmtContext* DOTParser::StmtContext::edge_stmt() {
  return getRuleContext<DOTParser::Edge_stmtContext>(0);
}

DOTParser::Attr_stmtContext* DOTParser::StmtContext::attr_stmt() {
  return getRuleContext<DOTParser::Attr_stmtContext>(0);
}

std::vector<DOTParser::IdContext *> DOTParser::StmtContext::id() {
  return getRuleContexts<DOTParser::IdContext>();
}

DOTParser::IdContext* DOTParser::StmtContext::id(size_t i) {
  return getRuleContext<DOTParser::IdContext>(i);
}

DOTParser::SubgraphContext* DOTParser::StmtContext::subgraph() {
  return getRuleContext<DOTParser::SubgraphContext>(0);
}


size_t DOTParser::StmtContext::getRuleIndex() const {
  return DOTParser::RuleStmt;
}

void DOTParser::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void DOTParser::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}


std::any DOTParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::StmtContext* DOTParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 4, DOTParser::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(56);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(48);
      node_stmt();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(49);
      edge_stmt();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(50);
      attr_stmt();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(51);
      id();
      setState(52);
      match(DOTParser::T__3);
      setState(53);
      id();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(55);
      subgraph();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Attr_stmtContext ------------------------------------------------------------------

DOTParser::Attr_stmtContext::Attr_stmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::Attr_listContext* DOTParser::Attr_stmtContext::attr_list() {
  return getRuleContext<DOTParser::Attr_listContext>(0);
}

tree::TerminalNode* DOTParser::Attr_stmtContext::GRAPH() {
  return getToken(DOTParser::GRAPH, 0);
}

tree::TerminalNode* DOTParser::Attr_stmtContext::NODE() {
  return getToken(DOTParser::NODE, 0);
}

tree::TerminalNode* DOTParser::Attr_stmtContext::EDGE() {
  return getToken(DOTParser::EDGE, 0);
}


size_t DOTParser::Attr_stmtContext::getRuleIndex() const {
  return DOTParser::RuleAttr_stmt;
}

void DOTParser::Attr_stmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttr_stmt(this);
}

void DOTParser::Attr_stmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttr_stmt(this);
}


std::any DOTParser::Attr_stmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitAttr_stmt(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Attr_stmtContext* DOTParser::attr_stmt() {
  Attr_stmtContext *_localctx = _tracker.createInstance<Attr_stmtContext>(_ctx, getState());
  enterRule(_localctx, 6, DOTParser::RuleAttr_stmt);
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
    setState(58);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 53248) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(59);
    attr_list();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Attr_listContext ------------------------------------------------------------------

DOTParser::Attr_listContext::Attr_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DOTParser::A_listContext *> DOTParser::Attr_listContext::a_list() {
  return getRuleContexts<DOTParser::A_listContext>();
}

DOTParser::A_listContext* DOTParser::Attr_listContext::a_list(size_t i) {
  return getRuleContext<DOTParser::A_listContext>(i);
}


size_t DOTParser::Attr_listContext::getRuleIndex() const {
  return DOTParser::RuleAttr_list;
}

void DOTParser::Attr_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttr_list(this);
}

void DOTParser::Attr_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttr_list(this);
}


std::any DOTParser::Attr_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitAttr_list(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Attr_listContext* DOTParser::attr_list() {
  Attr_listContext *_localctx = _tracker.createInstance<Attr_listContext>(_ctx, getState());
  enterRule(_localctx, 8, DOTParser::RuleAttr_list);
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
    setState(66); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(61);
      match(DOTParser::T__4);
      setState(63);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1966080) != 0) {
        setState(62);
        a_list();
      }
      setState(65);
      match(DOTParser::T__5);
      setState(68); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == DOTParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- A_listContext ------------------------------------------------------------------

DOTParser::A_listContext::A_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DOTParser::IdContext *> DOTParser::A_listContext::id() {
  return getRuleContexts<DOTParser::IdContext>();
}

DOTParser::IdContext* DOTParser::A_listContext::id(size_t i) {
  return getRuleContext<DOTParser::IdContext>(i);
}


size_t DOTParser::A_listContext::getRuleIndex() const {
  return DOTParser::RuleA_list;
}

void DOTParser::A_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterA_list(this);
}

void DOTParser::A_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitA_list(this);
}


std::any DOTParser::A_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitA_list(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::A_listContext* DOTParser::a_list() {
  A_listContext *_localctx = _tracker.createInstance<A_listContext>(_ctx, getState());
  enterRule(_localctx, 10, DOTParser::RuleA_list);
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
    setState(78); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(70);
      id();
      setState(73);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == DOTParser::T__3) {
        setState(71);
        match(DOTParser::T__3);
        setState(72);
        id();
      }
      setState(76);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == DOTParser::T__6) {
        setState(75);
        match(DOTParser::T__6);
      }
      setState(80); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1966080) != 0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Edge_stmtContext ------------------------------------------------------------------

DOTParser::Edge_stmtContext::Edge_stmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::EdgeRHSContext* DOTParser::Edge_stmtContext::edgeRHS() {
  return getRuleContext<DOTParser::EdgeRHSContext>(0);
}

DOTParser::Node_idContext* DOTParser::Edge_stmtContext::node_id() {
  return getRuleContext<DOTParser::Node_idContext>(0);
}

DOTParser::SubgraphContext* DOTParser::Edge_stmtContext::subgraph() {
  return getRuleContext<DOTParser::SubgraphContext>(0);
}

DOTParser::Attr_listContext* DOTParser::Edge_stmtContext::attr_list() {
  return getRuleContext<DOTParser::Attr_listContext>(0);
}


size_t DOTParser::Edge_stmtContext::getRuleIndex() const {
  return DOTParser::RuleEdge_stmt;
}

void DOTParser::Edge_stmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEdge_stmt(this);
}

void DOTParser::Edge_stmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEdge_stmt(this);
}


std::any DOTParser::Edge_stmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitEdge_stmt(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Edge_stmtContext* DOTParser::edge_stmt() {
  Edge_stmtContext *_localctx = _tracker.createInstance<Edge_stmtContext>(_ctx, getState());
  enterRule(_localctx, 12, DOTParser::RuleEdge_stmt);
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
    setState(84);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DOTParser::NUMBER:
      case DOTParser::STRING:
      case DOTParser::ID:
      case DOTParser::HTML_STRING: {
        setState(82);
        node_id();
        break;
      }

      case DOTParser::T__0:
      case DOTParser::SUBGRAPH: {
        setState(83);
        subgraph();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(86);
    edgeRHS();
    setState(88);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::T__4) {
      setState(87);
      attr_list();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EdgeRHSContext ------------------------------------------------------------------

DOTParser::EdgeRHSContext::EdgeRHSContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DOTParser::EdgeopContext *> DOTParser::EdgeRHSContext::edgeop() {
  return getRuleContexts<DOTParser::EdgeopContext>();
}

DOTParser::EdgeopContext* DOTParser::EdgeRHSContext::edgeop(size_t i) {
  return getRuleContext<DOTParser::EdgeopContext>(i);
}

std::vector<DOTParser::Node_idContext *> DOTParser::EdgeRHSContext::node_id() {
  return getRuleContexts<DOTParser::Node_idContext>();
}

DOTParser::Node_idContext* DOTParser::EdgeRHSContext::node_id(size_t i) {
  return getRuleContext<DOTParser::Node_idContext>(i);
}

std::vector<DOTParser::SubgraphContext *> DOTParser::EdgeRHSContext::subgraph() {
  return getRuleContexts<DOTParser::SubgraphContext>();
}

DOTParser::SubgraphContext* DOTParser::EdgeRHSContext::subgraph(size_t i) {
  return getRuleContext<DOTParser::SubgraphContext>(i);
}


size_t DOTParser::EdgeRHSContext::getRuleIndex() const {
  return DOTParser::RuleEdgeRHS;
}

void DOTParser::EdgeRHSContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEdgeRHS(this);
}

void DOTParser::EdgeRHSContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEdgeRHS(this);
}


std::any DOTParser::EdgeRHSContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitEdgeRHS(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::EdgeRHSContext* DOTParser::edgeRHS() {
  EdgeRHSContext *_localctx = _tracker.createInstance<EdgeRHSContext>(_ctx, getState());
  enterRule(_localctx, 14, DOTParser::RuleEdgeRHS);
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
    setState(95); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(90);
      edgeop();
      setState(93);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case DOTParser::NUMBER:
        case DOTParser::STRING:
        case DOTParser::ID:
        case DOTParser::HTML_STRING: {
          setState(91);
          node_id();
          break;
        }

        case DOTParser::T__0:
        case DOTParser::SUBGRAPH: {
          setState(92);
          subgraph();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(97); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == DOTParser::T__7

    || _la == DOTParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EdgeopContext ------------------------------------------------------------------

DOTParser::EdgeopContext::EdgeopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t DOTParser::EdgeopContext::getRuleIndex() const {
  return DOTParser::RuleEdgeop;
}

void DOTParser::EdgeopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEdgeop(this);
}

void DOTParser::EdgeopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEdgeop(this);
}


std::any DOTParser::EdgeopContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitEdgeop(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::EdgeopContext* DOTParser::edgeop() {
  EdgeopContext *_localctx = _tracker.createInstance<EdgeopContext>(_ctx, getState());
  enterRule(_localctx, 16, DOTParser::RuleEdgeop);
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
    setState(99);
    _la = _input->LA(1);
    if (!(_la == DOTParser::T__7

    || _la == DOTParser::T__8)) {
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

//----------------- Node_stmtContext ------------------------------------------------------------------

DOTParser::Node_stmtContext::Node_stmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::Node_idContext* DOTParser::Node_stmtContext::node_id() {
  return getRuleContext<DOTParser::Node_idContext>(0);
}

DOTParser::Attr_listContext* DOTParser::Node_stmtContext::attr_list() {
  return getRuleContext<DOTParser::Attr_listContext>(0);
}


size_t DOTParser::Node_stmtContext::getRuleIndex() const {
  return DOTParser::RuleNode_stmt;
}

void DOTParser::Node_stmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNode_stmt(this);
}

void DOTParser::Node_stmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNode_stmt(this);
}


std::any DOTParser::Node_stmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitNode_stmt(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Node_stmtContext* DOTParser::node_stmt() {
  Node_stmtContext *_localctx = _tracker.createInstance<Node_stmtContext>(_ctx, getState());
  enterRule(_localctx, 18, DOTParser::RuleNode_stmt);
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
    setState(101);
    node_id();
    setState(103);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::T__4) {
      setState(102);
      attr_list();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Node_idContext ------------------------------------------------------------------

DOTParser::Node_idContext::Node_idContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::IdContext* DOTParser::Node_idContext::id() {
  return getRuleContext<DOTParser::IdContext>(0);
}

DOTParser::PortContext* DOTParser::Node_idContext::port() {
  return getRuleContext<DOTParser::PortContext>(0);
}


size_t DOTParser::Node_idContext::getRuleIndex() const {
  return DOTParser::RuleNode_id;
}

void DOTParser::Node_idContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNode_id(this);
}

void DOTParser::Node_idContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNode_id(this);
}


std::any DOTParser::Node_idContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitNode_id(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::Node_idContext* DOTParser::node_id() {
  Node_idContext *_localctx = _tracker.createInstance<Node_idContext>(_ctx, getState());
  enterRule(_localctx, 20, DOTParser::RuleNode_id);
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
    setState(105);
    id();
    setState(107);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::T__9) {
      setState(106);
      port();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PortContext ------------------------------------------------------------------

DOTParser::PortContext::PortContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DOTParser::IdContext *> DOTParser::PortContext::id() {
  return getRuleContexts<DOTParser::IdContext>();
}

DOTParser::IdContext* DOTParser::PortContext::id(size_t i) {
  return getRuleContext<DOTParser::IdContext>(i);
}


size_t DOTParser::PortContext::getRuleIndex() const {
  return DOTParser::RulePort;
}

void DOTParser::PortContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPort(this);
}

void DOTParser::PortContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPort(this);
}


std::any DOTParser::PortContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitPort(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::PortContext* DOTParser::port() {
  PortContext *_localctx = _tracker.createInstance<PortContext>(_ctx, getState());
  enterRule(_localctx, 22, DOTParser::RulePort);
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
    setState(109);
    match(DOTParser::T__9);
    setState(110);
    id();
    setState(113);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::T__9) {
      setState(111);
      match(DOTParser::T__9);
      setState(112);
      id();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SubgraphContext ------------------------------------------------------------------

DOTParser::SubgraphContext::SubgraphContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DOTParser::Stmt_listContext* DOTParser::SubgraphContext::stmt_list() {
  return getRuleContext<DOTParser::Stmt_listContext>(0);
}

tree::TerminalNode* DOTParser::SubgraphContext::SUBGRAPH() {
  return getToken(DOTParser::SUBGRAPH, 0);
}

DOTParser::IdContext* DOTParser::SubgraphContext::id() {
  return getRuleContext<DOTParser::IdContext>(0);
}


size_t DOTParser::SubgraphContext::getRuleIndex() const {
  return DOTParser::RuleSubgraph;
}

void DOTParser::SubgraphContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSubgraph(this);
}

void DOTParser::SubgraphContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSubgraph(this);
}


std::any DOTParser::SubgraphContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitSubgraph(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::SubgraphContext* DOTParser::subgraph() {
  SubgraphContext *_localctx = _tracker.createInstance<SubgraphContext>(_ctx, getState());
  enterRule(_localctx, 24, DOTParser::RuleSubgraph);
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
    setState(119);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DOTParser::SUBGRAPH) {
      setState(115);
      match(DOTParser::SUBGRAPH);
      setState(117);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 1966080) != 0) {
        setState(116);
        id();
      }
    }
    setState(121);
    match(DOTParser::T__0);
    setState(122);
    stmt_list();
    setState(123);
    match(DOTParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdContext ------------------------------------------------------------------

DOTParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DOTParser::IdContext::ID() {
  return getToken(DOTParser::ID, 0);
}

tree::TerminalNode* DOTParser::IdContext::STRING() {
  return getToken(DOTParser::STRING, 0);
}

tree::TerminalNode* DOTParser::IdContext::HTML_STRING() {
  return getToken(DOTParser::HTML_STRING, 0);
}

tree::TerminalNode* DOTParser::IdContext::NUMBER() {
  return getToken(DOTParser::NUMBER, 0);
}


size_t DOTParser::IdContext::getRuleIndex() const {
  return DOTParser::RuleId;
}

void DOTParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void DOTParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DOTListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}


std::any DOTParser::IdContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<DOTVisitor*>(visitor))
    return parserVisitor->visitId(this);
  else
    return visitor->visitChildren(this);
}

DOTParser::IdContext* DOTParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 26, DOTParser::RuleId);
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
    setState(125);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1966080) != 0)) {
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

void DOTParser::initialize() {
  ::antlr4::internal::call_once(dotParserOnceFlag, dotParserInitialize);
}
