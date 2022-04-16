/*
 * DataTraceParse.h
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

#ifndef BPM21_DATATRACEPARSE_H
#define BPM21_DATATRACEPARSE_H

namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
}
class TraceLexer;
class TraceParser;
#include <yaucl/bpm/structures/log/TracesBaseVisitor.h>
//#include <yaucl/bpm/bpm_log.h>
#include <sstream>

#include <fstream>

#include <vector>
#include <variant>
#include <unordered_map>
#include "trace_visitor.h"

namespace yaucl {
    namespace bpm {
        class DataTraceParse : public TracesBaseVisitor {
            trace_visitor* tv ;
            bool isPayloadTrace;
            size_t trace_count = 0;
            size_t event_count = 0;
        public:
            bool load_also_data;
            DataTraceParse() : tv{nullptr}, load_also_data{true} {}
            ~DataTraceParse() {}

            void load(const std::string &stream_name, std::istream &stream, trace_visitor *tv);
            antlrcpp::Any visitLog(TracesParser::LogContext *ctx) override;
            antlrcpp::Any visitTrace(TracesParser::TraceContext *ctx) override;
            antlrcpp::Any visitEvent(TracesParser::EventContext *ctx) override;
            antlrcpp::Any visitData_part(TracesParser::Data_partContext *ctx) override;
            antlrcpp::Any visitField(TracesParser::FieldContext *ctx) override;
        };

    }
}


#endif //BPM21_DATATRACEPARSE_H
