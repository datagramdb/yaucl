
// Generated from include/yaucl/graphs/FLLOATProp.g4 by ANTLR 4.11.1


#include "yaucl/graphs/flloat/FLLOATPropLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct FLLOATPropLexerStaticData final {
  FLLOATPropLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FLLOATPropLexerStaticData(const FLLOATPropLexerStaticData&) = delete;
  FLLOATPropLexerStaticData(FLLOATPropLexerStaticData&&) = delete;
  FLLOATPropLexerStaticData& operator=(const FLLOATPropLexerStaticData&) = delete;
  FLLOATPropLexerStaticData& operator=(FLLOATPropLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag flloatproplexerLexerOnceFlag;
FLLOATPropLexerStaticData *flloatproplexerLexerStaticData = nullptr;

void flloatproplexerLexerInitialize() {
  assert(flloatproplexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<FLLOATPropLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "NUMBER", 
      "DIGIT", "ID", "LETTER", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'~'", "'('", "')'", "'&'", "'|'", "'True'", "'False'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "NUMBER", "ID", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,10,90,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,1,0,1,0,1,1,1,1,1,2,1,2,
  	1,3,1,3,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,1,6,1,6,1,6,1,7,3,7,48,
  	8,7,1,7,1,7,4,7,52,8,7,11,7,12,7,53,1,7,4,7,57,8,7,11,7,12,7,58,1,7,1,
  	7,5,7,63,8,7,10,7,12,7,66,9,7,3,7,68,8,7,3,7,70,8,7,1,8,1,8,1,9,1,9,1,
  	9,5,9,77,8,9,10,9,12,9,80,9,9,1,10,1,10,1,11,4,11,85,8,11,11,11,12,11,
  	86,1,11,1,11,0,0,12,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,0,19,9,21,0,
  	23,10,1,0,3,1,0,48,57,4,0,65,90,95,95,97,122,128,255,3,0,9,10,13,13,32,
  	32,96,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,
  	1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,19,1,0,0,0,0,23,1,0,0,0,1,25,1,0,
  	0,0,3,27,1,0,0,0,5,29,1,0,0,0,7,31,1,0,0,0,9,33,1,0,0,0,11,35,1,0,0,0,
  	13,40,1,0,0,0,15,47,1,0,0,0,17,71,1,0,0,0,19,73,1,0,0,0,21,81,1,0,0,0,
  	23,84,1,0,0,0,25,26,5,126,0,0,26,2,1,0,0,0,27,28,5,40,0,0,28,4,1,0,0,
  	0,29,30,5,41,0,0,30,6,1,0,0,0,31,32,5,38,0,0,32,8,1,0,0,0,33,34,5,124,
  	0,0,34,10,1,0,0,0,35,36,5,84,0,0,36,37,5,114,0,0,37,38,5,117,0,0,38,39,
  	5,101,0,0,39,12,1,0,0,0,40,41,5,70,0,0,41,42,5,97,0,0,42,43,5,108,0,0,
  	43,44,5,115,0,0,44,45,5,101,0,0,45,14,1,0,0,0,46,48,5,45,0,0,47,46,1,
  	0,0,0,47,48,1,0,0,0,48,69,1,0,0,0,49,51,5,46,0,0,50,52,3,17,8,0,51,50,
  	1,0,0,0,52,53,1,0,0,0,53,51,1,0,0,0,53,54,1,0,0,0,54,70,1,0,0,0,55,57,
  	3,17,8,0,56,55,1,0,0,0,57,58,1,0,0,0,58,56,1,0,0,0,58,59,1,0,0,0,59,67,
  	1,0,0,0,60,64,5,46,0,0,61,63,3,17,8,0,62,61,1,0,0,0,63,66,1,0,0,0,64,
  	62,1,0,0,0,64,65,1,0,0,0,65,68,1,0,0,0,66,64,1,0,0,0,67,60,1,0,0,0,67,
  	68,1,0,0,0,68,70,1,0,0,0,69,49,1,0,0,0,69,56,1,0,0,0,70,16,1,0,0,0,71,
  	72,7,0,0,0,72,18,1,0,0,0,73,78,3,21,10,0,74,77,3,21,10,0,75,77,3,17,8,
  	0,76,74,1,0,0,0,76,75,1,0,0,0,77,80,1,0,0,0,78,76,1,0,0,0,78,79,1,0,0,
  	0,79,20,1,0,0,0,80,78,1,0,0,0,81,82,7,1,0,0,82,22,1,0,0,0,83,85,7,2,0,
  	0,84,83,1,0,0,0,85,86,1,0,0,0,86,84,1,0,0,0,86,87,1,0,0,0,87,88,1,0,0,
  	0,88,89,6,11,0,0,89,24,1,0,0,0,10,0,47,53,58,64,67,69,76,78,86,1,6,0,
  	0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  flloatproplexerLexerStaticData = staticData.release();
}

}

FLLOATPropLexer::FLLOATPropLexer(CharStream *input) : Lexer(input) {
  FLLOATPropLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *flloatproplexerLexerStaticData->atn, flloatproplexerLexerStaticData->decisionToDFA, flloatproplexerLexerStaticData->sharedContextCache);
}

FLLOATPropLexer::~FLLOATPropLexer() {
  delete _interpreter;
}

std::string FLLOATPropLexer::getGrammarFileName() const {
  return "FLLOATProp.g4";
}

const std::vector<std::string>& FLLOATPropLexer::getRuleNames() const {
  return flloatproplexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& FLLOATPropLexer::getChannelNames() const {
  return flloatproplexerLexerStaticData->channelNames;
}

const std::vector<std::string>& FLLOATPropLexer::getModeNames() const {
  return flloatproplexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& FLLOATPropLexer::getVocabulary() const {
  return flloatproplexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView FLLOATPropLexer::getSerializedATN() const {
  return flloatproplexerLexerStaticData->serializedATN;
}

const atn::ATN& FLLOATPropLexer::getATN() const {
  return *flloatproplexerLexerStaticData->atn;
}




void FLLOATPropLexer::initialize() {
  ::antlr4::internal::call_once(flloatproplexerLexerOnceFlag, flloatproplexerLexerInitialize);
}
