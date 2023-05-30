//
// Created by giacomo on 23/01/23.
//

#include <yaucl/graphs/flloat/FLLOATSimplePropParser.h>
/*
 * FLLOATSimplePropParser.cpp
 * This file is part of bpm21
 *
 * Copyright (C) 2021 - Giacomo Bergami
 *
 * bpm21 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * bpm21 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bpm21. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 13/03/21.
//

//#include "knobab/flloat_deps/FLLOATSimplePropParser.h"
#include <yaucl/graphs/flloat//FLLOATPropLexer.h>
#include <yaucl/bpm/structures/commons/easy_prop.h>
#include <yaucl/functional/assert.h>

easy_prop FLLOATSimplePropParser::parse(std::istream &stream) {
    antlr4::ANTLRInputStream input(stream);
    FLLOATPropLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();
    FLLOATPropParser parser(&tokens);
    // Returning the simplified formula, thus in DFN and with the negation pushed towards the leaves
    return std::any_cast<easy_prop>(visit(parser.statement()))/*.nnf().simplify()*//*.reduce().oversimplify()*/;
}

std::any FLLOATSimplePropParser::visitAtom(FLLOATPropParser::AtomContext *context) {
    if (context) {
        easy_prop ep;
        ep.casusu = easy_prop::E_P_ATOM;
        ep.isAtomNegated = false;
        ep.single_atom_if_any = context->getText();
        return {ep};
    }
    return {easy_prop{easy_prop::E_P_FALSE}};
}

std::any FLLOATSimplePropParser::visitNegation(FLLOATPropParser::NegationContext *context) {
    if (context) {
        auto tmp = std::any_cast<easy_prop>(visit(context->statement()));
        DEBUG_ASSERT((tmp.casusu == easy_prop::E_P_ATOM) && (!tmp.isAtomNegated));
        tmp.isAtomNegated = true;
        return {tmp};
    }
    return {easy_prop{easy_prop::E_P_FALSE}};
}

std::any FLLOATSimplePropParser::visitParen(FLLOATPropParser::ParenContext *context) {
    if (context) {
        return visit(context->statement());
    }
    return {easy_prop{easy_prop::E_P_FALSE}};
}

std::any FLLOATSimplePropParser::visitOr(FLLOATPropParser::OrContext *context) {
    if (context) {
        DEBUG_ASSERT(context->statement().size() == 2);
        easy_prop ep;
        ep.casusu = easy_prop::E_P_OR;
        ep.isAtomNegated = false;
        ep.args.emplace_back(std::any_cast<easy_prop>(visit(context->statement(0))));
        ep.args.emplace_back(std::any_cast<easy_prop>(visit(context->statement(1))));
        return ep;
    }
    return {easy_prop{easy_prop::E_P_FALSE}};
}

std::any FLLOATSimplePropParser::visitTop(FLLOATPropParser::TopContext *context) {
    return {easy_prop{}};
}

std::any FLLOATSimplePropParser::visitBot(FLLOATPropParser::BotContext *context) {
    return {easy_prop{easy_prop::E_P_FALSE}};
}

std::any FLLOATSimplePropParser::visitAnd(FLLOATPropParser::AndContext *context) {
    if (context) {
        DEBUG_ASSERT(context->statement().size() == 2);
        easy_prop ep;
        ep.casusu = easy_prop::E_P_AND;
        ep.isAtomNegated = false;
        ep.args.emplace_back(std::any_cast<easy_prop>(visit(context->statement(0))));
        ep.args.emplace_back(std::any_cast<easy_prop>(visit(context->statement(1))));
        return ep;
    }
    return {ltlf::True().negate()};
}
