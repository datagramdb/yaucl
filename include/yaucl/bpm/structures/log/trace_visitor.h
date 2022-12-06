//
// Created by giacomo on 31/10/2021.
//

#ifndef BZDB_TRACE_VISITOR_H
#define BZDB_TRACE_VISITOR_H

#include <string>

struct trace_visitor {
        virtual void enterLog(const std::string& source, const std::string& name) = 0;
        virtual void exitLog(const std::string& source, const std::string& name) = 0;

        virtual size_t enterTrace(const std::string &trace_label) = 0;
        virtual void exitTrace(size_t traceId) = 0;

        virtual size_t enterEvent(size_t chronos_tick, const std::string &event_label) = 0;
        virtual void exitEvent(size_t event_id) = 0;

        virtual void enterData_part(bool isEvent) = 0;
        virtual void exitData_part(bool isEvent) = 0;

        virtual void visitField(const std::string& key, bool value) = 0;
        virtual void visitField(const std::string& key, double value) = 0;
        virtual void visitField(const std::string& key, const std::string& value) = 0;
        virtual void visitField(const std::string& key, size_t value) = 0;
};


#endif //BZDB_TRACE_VISITOR_H
