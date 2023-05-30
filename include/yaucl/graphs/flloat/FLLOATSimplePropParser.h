//
// Created by giacomo on 23/01/23.
//

#ifndef KNOBAB_SERVER_FLLOATSIMPLEPROPPARSER_H
#define KNOBAB_SERVER_FLLOATSIMPLEPROPPARSER_H

namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
}
class FLLOATPropLexer;
class FLLOATPropParser;
#include <yaucl/graphs/flloat/FLLOATPropVisitor.h>
#include <sstream>
#include <magic_enum.hpp>
#include <fstream>
#ifndef CLASSIFIERS_PAIR_HASH_H
#define CLASSIFIERS_PAIR_HASH_H
#define CLASSIFIERS_PAIR_HASH_H_redef
#endif
#include <yaucl/bpm/structures/ltlf/ltlf.h>
#include <yaucl/bpm/structures/commons/easy_prop.h>


/**
 * Propositions parses from the FLLOAT output, using a specific sympy format
 */
class FLLOATSimplePropParser : public FLLOATPropVisitor {

public:
    FLLOATSimplePropParser(){}
    ~FLLOATSimplePropParser() {}

    easy_prop parse(std::istream& stream);

    std::any visitNegation(FLLOATPropParser::NegationContext *context) override;
    std::any visitAtom(FLLOATPropParser::AtomContext *context) override;
    std::any visitParen(FLLOATPropParser::ParenContext *context) override;
    std::any visitOr(FLLOATPropParser::OrContext *context) override;
    std::any visitTop(FLLOATPropParser::TopContext *context) override;
    std::any visitBot(FLLOATPropParser::BotContext *context) override;
    std::any visitAnd(FLLOATPropParser::AndContext *context) override;
};

#ifdef CLASSIFIERS_PAIR_HASH_H_redef
#undef CLASSIFIERS_PAIR_HASH_H_redef
#endif

#endif //KNOBAB_SERVER_FLLOATSIMPLEPROPPARSER_H
