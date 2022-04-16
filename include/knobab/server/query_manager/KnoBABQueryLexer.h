
// Generated from KnoBABQuery.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"




class  KnoBABQueryLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, ACT_TABLE = 59, CNT_TABLE = 60, ATT_TABLE = 61, 
    ATT = 62, ACTIVATION = 63, TARGET = 64, INIT = 65, END = 66, EXISTS = 67, 
    ABSENCE = 68, NEXT = 69, OR = 70, AND = 71, FIRST = 72, LAST = 73, IF = 74, 
    THEN = 75, ELSE = 76, UNTIL = 77, BOX = 78, DIAMOND = 79, AUTO_TIMED = 80, 
    LPAREN = 81, RPAREN = 82, PRESERVE = 83, TIMED = 84, THETA = 85, LEFT = 86, 
    RIGHT = 87, MIDDLE = 88, NEGATED = 89, HRF = 90, TAB = 91, XES = 92, 
    LABEL = 93, INTNUMBER = 94, NUMBER = 95, STRING = 96, SPACE = 97, COMMENT = 98, 
    LINE_COMMENT = 99
  };

  explicit KnoBABQueryLexer(antlr4::CharStream *input);

  ~KnoBABQueryLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

