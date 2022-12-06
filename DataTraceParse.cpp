/*
 * DataTraceParse.cpp
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
// Created by giacomo on 12/03/21.
//

#include <yaucl/bpm/DataTraceParse.h>
#include <yaucl/bpm/TracesLexer.h>
#include <yaucl/bpm/TracesParser.h>

//using namespace yaucl::bpm;

yaucl::bpm::log yaucl::bpm::DataTraceParse::load(const std::string& stream_name, std::istream& stream) {

    antlr4::ANTLRInputStream input(stream);
    TracesLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    TracesParser parser(&tokens);
    return visitLog(parser.log()).as<yaucl::bpm::log>();
}

antlrcpp::Any yaucl::bpm::DataTraceParse::visitLog(TracesParser::LogContext *ctx) {
    yaucl::bpm::log log;
    if (ctx) {
        size_t trace_id = 1;
        for (const auto& ptr : ctx->trace()) {
            auto it = visitTrace(ptr);
            if (it.isNotNull()) {
                auto t = it.as<yaucl::bpm::trace>();
                t.event_label_or_name = std::to_string(trace_id);
                log.traces.emplace_back(t);
                trace_id++;
            }
        }
    }
    return {log};
}

antlrcpp::Any yaucl::bpm::DataTraceParse::visitTrace(TracesParser::TraceContext *ctx) {
    yaucl::bpm::trace trace;
    if (ctx) {
        size_t event_count = 1;
        trace.payload = visitData_part(ctx->data_part()).as<std::unordered_map<std::string, std::variant<std::string, double>>>();
        for (const auto& ptr : ctx->event()) {
            auto it = visitEvent(ptr);
            if (it.isNotNull()) {
                auto e = it.as<yaucl::bpm::event>();
                e.epoch_time_and_event_id = {event_count, event_count};
                event_count++;
                trace.trace_events.emplace_back(e);
            }
        }

    }
    return {trace};
}

antlrcpp::Any yaucl::bpm::DataTraceParse::visitEvent(TracesParser::EventContext *ctx) {
    if (ctx) {
        yaucl::bpm::event event;
        event.event_label_or_name = ctx->LABEL()->getText();
        /// TODO: std::transform(event.first.begin(), event.first.end(), event.first.begin(), ::tolower);
        event.payload = visitData_part(ctx->data_part()).as<std::unordered_map<std::string, std::variant<std::string, double>>>();
        return {event};
    }
    return {};
}

antlrcpp::Any yaucl::bpm::DataTraceParse::visitData_part(TracesParser::Data_partContext *ctx) {
    std::unordered_map<std::string, std::variant<std::string, double>> map;
    if (ctx) {
        for (const auto& ptr : ctx->field()) {
            auto it = visitField(ptr).as<std::unordered_map<std::string, std::variant<std::string, double>>>();
            map.insert(it.begin(), it.end());
        }
    }
    return {map};
}

#include "yaucl/strings/stringutils.h"

antlrcpp::Any yaucl::bpm::DataTraceParse::visitField(TracesParser::FieldContext *ctx) {
    std::unordered_map<std::string, std::variant<std::string, double>> map;
    if (ctx) {
        if (ctx->NUMBER()) {
            map[ctx->VAR()->getText()] = std::stod(ctx->NUMBER()->getText());
        } else {
            assert(ctx->STRING());
            map[ctx->VAR()->getText()] = UNESCAPE(ctx->STRING()->getText());
        }
    }
    return {map};
}
