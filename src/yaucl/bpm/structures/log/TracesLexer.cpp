
// Generated from src/yaucl/bpm/structures/log/Traces.g4 by ANTLR 4.10.1


#include "yaucl/bpm/structures/log/TracesLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct TracesLexerStaticData final {
  TracesLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  TracesLexerStaticData(const TracesLexerStaticData&) = delete;
  TracesLexerStaticData(TracesLexerStaticData&&) = delete;
  TracesLexerStaticData& operator=(const TracesLexerStaticData&) = delete;
  TracesLexerStaticData& operator=(TracesLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

std::once_flag traceslexerLexerOnceFlag;
TracesLexerStaticData *traceslexerLexerStaticData = nullptr;

void traceslexerLexerInitialize() {
  assert(traceslexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<TracesLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "VAR", "LABEL", "INTNUMBER", 
      "NUMBER", "STRING", "SPACE", "COMMENT", "LINE_COMMENT"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
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
  	4,0,13,105,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,1,0,1,0,1,1,1,
  	1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,4,5,39,8,5,11,5,12,5,40,1,6,1,6,5,6,45,
  	8,6,10,6,12,6,48,9,6,1,7,4,7,51,8,7,11,7,12,7,52,1,8,3,8,56,8,8,1,8,1,
  	8,1,8,3,8,61,8,8,1,9,1,9,1,9,1,9,5,9,67,8,9,10,9,12,9,70,9,9,1,9,1,9,
  	1,10,4,10,75,8,10,11,10,12,10,76,1,10,1,10,1,11,1,11,1,11,1,11,5,11,85,
  	8,11,10,11,12,11,88,9,11,1,11,1,11,1,11,1,11,1,11,1,12,1,12,1,12,1,12,
  	5,12,99,8,12,10,12,12,12,102,9,12,1,12,1,12,1,86,0,13,1,1,3,2,5,3,7,4,
  	9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,1,0,4,2,0,65,90,97,122,
  	2,0,34,34,92,92,3,0,9,10,13,13,32,32,2,0,10,10,13,13,114,0,1,1,0,0,0,
  	0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,
  	0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,
  	0,25,1,0,0,0,1,27,1,0,0,0,3,29,1,0,0,0,5,31,1,0,0,0,7,33,1,0,0,0,9,35,
  	1,0,0,0,11,38,1,0,0,0,13,42,1,0,0,0,15,50,1,0,0,0,17,55,1,0,0,0,19,62,
  	1,0,0,0,21,74,1,0,0,0,23,80,1,0,0,0,25,94,1,0,0,0,27,28,5,59,0,0,28,2,
  	1,0,0,0,29,30,5,58,0,0,30,4,1,0,0,0,31,32,5,123,0,0,32,6,1,0,0,0,33,34,
  	5,125,0,0,34,8,1,0,0,0,35,36,5,61,0,0,36,10,1,0,0,0,37,39,2,97,122,0,
  	38,37,1,0,0,0,39,40,1,0,0,0,40,38,1,0,0,0,40,41,1,0,0,0,41,12,1,0,0,0,
  	42,46,2,65,90,0,43,45,7,0,0,0,44,43,1,0,0,0,45,48,1,0,0,0,46,44,1,0,0,
  	0,46,47,1,0,0,0,47,14,1,0,0,0,48,46,1,0,0,0,49,51,2,48,57,0,50,49,1,0,
  	0,0,51,52,1,0,0,0,52,50,1,0,0,0,52,53,1,0,0,0,53,16,1,0,0,0,54,56,5,45,
  	0,0,55,54,1,0,0,0,55,56,1,0,0,0,56,57,1,0,0,0,57,60,3,15,7,0,58,59,5,
  	46,0,0,59,61,3,15,7,0,60,58,1,0,0,0,60,61,1,0,0,0,61,18,1,0,0,0,62,68,
  	5,34,0,0,63,67,8,1,0,0,64,65,5,92,0,0,65,67,7,1,0,0,66,63,1,0,0,0,66,
  	64,1,0,0,0,67,70,1,0,0,0,68,66,1,0,0,0,68,69,1,0,0,0,69,71,1,0,0,0,70,
  	68,1,0,0,0,71,72,5,34,0,0,72,20,1,0,0,0,73,75,7,2,0,0,74,73,1,0,0,0,75,
  	76,1,0,0,0,76,74,1,0,0,0,76,77,1,0,0,0,77,78,1,0,0,0,78,79,6,10,0,0,79,
  	22,1,0,0,0,80,81,5,47,0,0,81,82,5,42,0,0,82,86,1,0,0,0,83,85,9,0,0,0,
  	84,83,1,0,0,0,85,88,1,0,0,0,86,87,1,0,0,0,86,84,1,0,0,0,87,89,1,0,0,0,
  	88,86,1,0,0,0,89,90,5,42,0,0,90,91,5,47,0,0,91,92,1,0,0,0,92,93,6,11,
  	0,0,93,24,1,0,0,0,94,95,5,47,0,0,95,96,5,47,0,0,96,100,1,0,0,0,97,99,
  	8,3,0,0,98,97,1,0,0,0,99,102,1,0,0,0,100,98,1,0,0,0,100,101,1,0,0,0,101,
  	103,1,0,0,0,102,100,1,0,0,0,103,104,6,12,0,0,104,26,1,0,0,0,11,0,40,46,
  	52,55,60,66,68,76,86,100,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  traceslexerLexerStaticData = staticData.release();
}

}

TracesLexer::TracesLexer(CharStream *input) : Lexer(input) {
  TracesLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *traceslexerLexerStaticData->atn, traceslexerLexerStaticData->decisionToDFA, traceslexerLexerStaticData->sharedContextCache);
}

TracesLexer::~TracesLexer() {
  delete _interpreter;
}

std::string TracesLexer::getGrammarFileName() const {
  return "Traces.g4";
}

const std::vector<std::string>& TracesLexer::getRuleNames() const {
  return traceslexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& TracesLexer::getChannelNames() const {
  return traceslexerLexerStaticData->channelNames;
}

const std::vector<std::string>& TracesLexer::getModeNames() const {
  return traceslexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& TracesLexer::getVocabulary() const {
  return traceslexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView TracesLexer::getSerializedATN() const {
  return traceslexerLexerStaticData->serializedATN;
}

const atn::ATN& TracesLexer::getATN() const {
  return *traceslexerLexerStaticData->atn;
}




void TracesLexer::initialize() {
  std::call_once(traceslexerLexerOnceFlag, traceslexerLexerInitialize);
}
