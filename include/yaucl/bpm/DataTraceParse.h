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
#include <yaucl/bpm/TracesBaseVisitor.h>
#include <yaucl/bpm/bpm_log.h>
#include <sstream>
#include <yaucl/strings/stringutils.h>
#include <fstream>

#include <vector>
#include <variant>
#include <unordered_map>

namespace yaucl {
    namespace bpm {
        class DataTraceParse : public TracesBaseVisitor {

        public:
            DataTraceParse() {}
            ~DataTraceParse() {}

            log load(const std::string& stream_name, std::istream& stream);
            antlrcpp::Any visitLog(TracesParser::LogContext *ctx) override;
            antlrcpp::Any visitTrace(TracesParser::TraceContext *ctx) override;
            antlrcpp::Any visitEvent(TracesParser::EventContext *ctx) override;
            antlrcpp::Any visitData_part(TracesParser::Data_partContext *ctx) override;
            antlrcpp::Any visitField(TracesParser::FieldContext *ctx) override;
        };

    }
}


#endif //BPM21_DATATRACEPARSE_H
