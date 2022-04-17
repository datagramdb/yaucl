//
// Created by giacomo on 15/04/2022.
//

#include <knobab/server/declare/DeclareDataAware.h>
#include <yaucl/functional/assert.h>
#include <yaucl/data/json.h>
#include "knobab/server/query_manager/ServerQueryManager.h"

std::any ServerQueryManager::visitData_aware_declare(KnoBABQueryParser::Data_aware_declareContext *ctx) {
    std::vector<DeclareDataAware> v;
    if (ctx) {
        for (KnoBABQueryParser::DeclareContext* ptr : ctx->declare()) {
            auto currVisit = visit(ptr);
            if (!currVisit.has_value()) {
                // If the log file has to be interpreted as a model, and therefore as conjunctive,
                // then since one clause fails, all the other will fail in conjunction: therefore,
                // the model is empty
                if (asConjunctiveModel)
                    return {std::vector<DeclareDataAware>{}};
                // Otherwise, discard the invalid element
            } else{
                // Add the valid element, nevertheless!
                v.emplace_back(std::any_cast<DeclareDataAware>(currVisit));
            }
        }
    }
    return {v};
}

std::any ServerQueryManager::visitNary_prop(KnoBABQueryParser::Nary_propContext *ctx) {
    DeclareDataAware dda;
    if (ctx) {
        auto leftAny = visitFields(ctx->fields(0));
        if (!leftAny.has_value())
            return {};
        std::tie (dda.left_act, dda.dnf_left_map) =
                std::any_cast<std::pair<std::string, std::vector<std::unordered_map<std::string, DataPredicate>>>>(leftAny);

        auto rightAny = visitFields(ctx->fields(1));
        if (!rightAny.has_value())
            return {};
        std::tie (dda.right_act, dda.dnf_right_map) =
                std::any_cast<std::pair<std::string,
                        std::vector<std::unordered_map<std::string, DataPredicate>>>>(rightAny);
        /*if (do_renaming)
            std::transform(dda.left_act.begin(), dda.left_act.end(), dda.left_act.begin(), ::tolower);*/
        for (auto& ref : dda.dnf_left_map) {
            for (auto& cp : ref) {
                cp.second.label = dda.left_act;
                for (auto& subitem : cp.second.BiVariableConditions) {
                    subitem.label = dda.left_act;
                    subitem.labelRHS = dda.left_act;
                }
                if (!cp.second.varRHS.empty()) {
                    cp.second.labelRHS = dda.left_act;
                }
            }
        }

        /*if (do_renaming)
            std::transform(dda.right_act.begin(), dda.right_act.end(), dda.right_act.begin(), ::tolower);*/
        for (auto& ref : dda.dnf_right_map) {
            for (auto& cp : ref) {
                cp.second.label = dda.right_act;
                for (auto& subitem : cp.second.BiVariableConditions) {
                    subitem.label = dda.right_act;
                    subitem.labelRHS = dda.right_act;
                }
                if (!cp.second.varRHS.empty()) {
                    cp.second.labelRHS = dda.right_act;
                }
            }
        }

        if (ctx->prop()) {
            auto conjAny = visit(ctx->prop());
            if (!conjAny.has_value())
                return {};
            dda.conjunctive_map = std::any_cast<std::vector<std::unordered_map<std::string, DataPredicate>>>(conjAny);
            for (auto& ref : dda.conjunctive_map) {
                for (auto& cp : ref) {
                    DEBUG_ASSERT(cp.second.casusu == numeric_atom_cases::TTRUE); // That is, no data interval should be provided in here!
                    cp.second.label = dda.left_act;
                    cp.second.labelRHS = dda.right_act;
                    for (auto& subitem : cp.second.BiVariableConditions) {
                        DEBUG_ASSERT(subitem.isBiVariableCondition()); // in the conjunction after where, all the predicates should be among variables, only!
                        subitem.label = dda.left_act;
                        subitem.labelRHS = dda.right_act;
                    }
                }
            }
        }

        dda.n = 0;
        dda.casusu = UNESCAPE(ctx->STRING()->getText());
    }
    return {dda};
}

std::any ServerQueryManager::visitUnary_prop(KnoBABQueryParser::Unary_propContext *ctx) {
    DeclareDataAware dda;
    if (ctx) {
        auto leftAny = visitFields(ctx->fields());
        if (!leftAny.has_value())
            return {};
        std::tie (dda.left_act, dda.dnf_left_map) =
                std::any_cast<std::pair<std::string, std::vector<std::unordered_map<std::string, DataPredicate>>>>(leftAny);
        for (auto& ref : dda.dnf_left_map) {
            for (auto& cp : ref) {
                cp.second.label = dda.left_act;
            }
        }
        dda.n = std::stoul(ctx->INTNUMBER()->getText());
        dda.casusu = UNESCAPE(ctx->STRING()->getText());
    }
    return {dda};
}

std::any ServerQueryManager::visitFields(KnoBABQueryParser::FieldsContext *ctx) {
    std::pair<std::string,
            std::vector<std::unordered_map<std::string, DataPredicate>>> cp;
    if (ctx) {
        auto prop = visit(ctx->prop());
        if (prop.has_value()) {
            std::pair<std::string,
                    std::vector<std::unordered_map<std::string, DataPredicate>>> cp;
            cp.first = UNESCAPE(ctx->STRING()->getText());
            cp.second = std::any_cast<std::vector<std::unordered_map<std::string, DataPredicate>>>(visit(ctx->prop()));
            return {cp};
        } else {
            return {};
        }
    }
    return {cp};
}

std::any ServerQueryManager::visitDisj(KnoBABQueryParser::DisjContext *ctx) {
    if (ctx) {
        auto M = std::any_cast<std::unordered_map<std::string, DataPredicate>>(visit(ctx->prop_within_dijunction()));
        auto vAny = visit(ctx->prop());
        if (vAny.has_value()) {
            auto v = std::any_cast<std::vector<std::unordered_map<std::string, DataPredicate>>>(vAny);
            if (v.empty()) {
                // if the element is empty, it means that it evaluates to true, so this can be immediately returned
                return vAny;
            } else {
                // Otherwise, I can add the conjunctive condition if it does not evaluate to false
                if (!M.empty())
                    v.emplace_back(M);
                return {v};
            }
        } else {
            // if it evaluates to false, it can be only changed by M
            if (!M.empty()) {
                std::vector<std::unordered_map<std::string, DataPredicate>> v;
                v.emplace_back(M);
                return {v};
            } else {
                // evaluates to false!
                return {};
            }
        }

    }
    return {};
}

std::any ServerQueryManager::visitConj_or_atom(KnoBABQueryParser::Conj_or_atomContext *ctx) {
    std::vector<std::unordered_map<std::string, DataPredicate>> v;
    if (ctx) {
        auto M = std::any_cast<std::unordered_map<std::string, DataPredicate>>(visit(ctx->prop_within_dijunction()));
        if (!M.empty()) {
            // inserting an element to the disjunction, only if the element is not empty
            // that is, only if it does not evaluate to false
            v.emplace_back(M);
            return {v};
        } else {
            // Otherwise, evaluate that to false as a whole
            return {};
        }
    }
    return {};
}

std::any ServerQueryManager::visitTop(KnoBABQueryParser::TopContext *context) {
    return {std::vector<std::unordered_map<std::string, DataPredicate>>{}};
}

std::any ServerQueryManager::visitIn_atom(KnoBABQueryParser::In_atomContext *ctx) {
    std::unordered_map<std::string, DataPredicate> v;
    if (ctx) {
        auto pred = std::any_cast<DataPredicate>(visitAtom(ctx->atom()));
        if (pred.isBiVariableCondition()) {
            v[pred.var].var = pred.var;
            v[pred.var].BiVariableConditions.emplace_back(pred);
        } else {
            v[pred.var] = pred;
        }

    }
    return {v};
}

std::any ServerQueryManager::visitAtom_conj(KnoBABQueryParser::Atom_conjContext *ctx) {
    std::unordered_map<std::string, DataPredicate> v;
    if (ctx) {
        v = std::any_cast<std::unordered_map<std::string, DataPredicate>>(visit(ctx->prop_within_dijunction()));
        // by the reason of grammar, if v is empty, it means that one of the predicates is empty
        // Given that we are in conjunction, this means all the elements of the conjunction will be
        // evaluated to false, too!
        if (v.empty())
            return {v};
        auto baseCase = std::any_cast<DataPredicate>(visitAtom(ctx->atom()));
        auto it = v.find(baseCase.var);
        if (it != v.end()) {
            if (!it->second.intersect_with(baseCase)) {
                // if the evaluation reduces to false, the whole conjunction is false! therefore, returning it
                v.clear();
            } // otherwise, the intersection is already effective!
        } else {
            if (baseCase.isBiVariableCondition()) {
                v[baseCase.var].var = baseCase.var;
                v[baseCase.var].BiVariableConditions.emplace_back(baseCase);
            } else {
                v[baseCase.var] = baseCase;
            }
        }
    }
    return {v};
}

std::any ServerQueryManager::visitAtom(KnoBABQueryParser::AtomContext *ctx) {
    if (ctx) {
        DataPredicate pred;
        auto vars = ctx->var();
        pred.var = UNESCAPE(vars.at(0)->STRING()->getText());
        pred.casusu = std::any_cast<numeric_atom_cases>(visit(ctx->rel()));
        if (ctx->STRING()) {
            pred.value = UNESCAPE(ctx->STRING()->getText());
        } else if (ctx->NUMBER()) {
            pred.value = std::stod(ctx->NUMBER()->getText());
        } else if (vars.size() > 1) {
            pred.varRHS = UNESCAPE(vars.at(1)->STRING()->getText());
        } else {
            pred.value = 0.0;
        }
        return {pred};
    }
    return {};
}

std::any ServerQueryManager::visitLt(KnoBABQueryParser::LtContext *ctx) {
    return {LT};
}

std::any ServerQueryManager::visitLeq(KnoBABQueryParser::LeqContext *ctx) {
    return {LEQ};
}

std::any ServerQueryManager::visitGt(KnoBABQueryParser::GtContext *ctx) {
    return {GT};
}

std::any ServerQueryManager::visitGeq(KnoBABQueryParser::GeqContext *ctx) {
    return {GEQ};
}

std::any ServerQueryManager::visitEq(KnoBABQueryParser::EqContext *ctx) {
    return {EQ};
}

std::any ServerQueryManager::visitNeq(KnoBABQueryParser::NeqContext *ctx) {
    return {NEQ};
}

std::any ServerQueryManager::visitLoad_data_query(KnoBABQueryParser::Load_data_queryContext *context) {
    if (context) {
        std::string filename = UNESCAPE(context->file->getText());
        bool oldLoading = true;
        load_also_data = (context->with_data());
        bool doStats = !(context->no_stats());
        bool index_missing_data = (context->with_missing());
        std::string env_name = UNESCAPE(context->env_name->getText());
        if (multiple_logs.contains(env_name)) {
            multiple_logs.erase(env_name);
        }
        auto& env = multiple_logs[env_name];
        log_data_format format;
        if (context->HRF()) {
            format = HUMAN_READABLE_YAUCL;
        } else if (context->TAB()) {
            format = TAB_SEPARATED_EVENTS;
        } else if (context->XES()) {
            format = XES1;
        } else if (context->log()) {
            oldLoading = false;
        }

        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::milliseconds;
        env.experiment_logger.log_filename = filename;

        {
            //log_data_format format, bool loadData, std::istream &stream, KnowledgeBase &output,
            //                              std::string &filename
            auto t1 = high_resolution_clock::now();
            if (oldLoading) {
                std::ifstream input_stream{filename};
                load_into_knowledge_base(format, load_also_data, input_stream, env.db, filename);
            } else {
                tv = &env.db;
                visitLog(context->log());
                tv = nullptr;
            }
            auto t2 = high_resolution_clock::now();

            /* Getting number of milliseconds as a double. */
            duration<double, std::milli> ms_double = t2 - t1;
            env.experiment_logger.log_loading_and_parsing_ms = ms_double.count();
            //std::cout << "Loading and parsing time = " << ms_double.count() << std::endl;
        }

        {
            auto t1 = high_resolution_clock::now();
            env.db.index_data_structures(index_missing_data);
            auto t2 = high_resolution_clock::now();

            /* Getting number of milliseconds as a double. */
            duration<double, std::milli> ms_double = t2 - t1;
            env.experiment_logger.log_indexing_ms = ms_double.count();
            //std::cout << "Indexing time = " << ms_double.count() << std::endl;
        }
        auto tmp = env.db.getMaximumStringLength();
        env.ap.s_max = std::string(tmp, std::numeric_limits<char>::max());
        DataPredicate::MAX_STRING = env.ap.s_max;
        DataPredicate::msl = tmp;

        env.experiment_logger.n_traces = env.db.noTraces;
        env.experiment_logger.n_acts = env.db.actId;
        // Compute some more trace statistics

        double trace_avg, trace_pow2, N;
        N = env.db.act_table_by_act_id.trace_length.size();
        size_t frequency_of_trace_length = 0;
        size_t previousLength = 0;
        std::multiset<size_t> O;

        if (doStats) {
            for (const size_t i : env.db.act_table_by_act_id.trace_length) {
                trace_avg += i;
                trace_pow2 += std::pow(i, 2);
                O.insert(i);
            }
            for (size_t len : O) {
                size_t currFreq = O.count(len);
                if (currFreq > frequency_of_trace_length) {
                    frequency_of_trace_length = currFreq;
                    previousLength = len;
                }
            }
            trace_avg = trace_avg / N;
            env.experiment_logger.log_trace_average_length = trace_avg;
            env.experiment_logger.log_trace_variance = (trace_pow2 / N) - std::pow(trace_avg, 2);
            env.experiment_logger.most_frequent_trace_length = previousLength;
            env.experiment_logger.trace_length_frequency = frequency_of_trace_length;
        }
        content.str(std::string());
        env.experiment_logger.log_json_file(content);
        infos.emplace_back(env.experiment_logger);
        this->format = "text/json";
    } else {
        content.str(std::string());
        content << "{}";
        this->format = "text/json";
    }
    return {};
}

std::any ServerQueryManager::visitField(KnoBABQueryParser::FieldContext *ctx) {
    if (ctx) {
        if (ctx->NUMBER()) {
            tv->visitField(
                    UNESCAPE(ctx->var()->STRING()->getText()), std::stod(ctx->NUMBER()->getText()));
        } else {
            DEBUG_ASSERT(ctx->STRING());
            tv->visitField(
                    UNESCAPE(ctx->var()->STRING()->getText()), UNESCAPE(ctx->STRING()->getText()));
        }
    }
    return {};
}

antlrcpp::Any ServerQueryManager::visitLog(KnoBABQueryParser::LogContext *ctx) {
    trace_count = 0;
    isPayloadTrace = false;
    if (ctx) {
        for (const auto& ptr : ctx->trace()) {
            visitTrace(ptr);
        }
    }
    return {};
}

antlrcpp::Any ServerQueryManager::visitTrace(KnoBABQueryParser::TraceContext *ctx) {
    if (ctx) {
        size_t event_count = 1;
        size_t traceId = tv->enterTrace(std::to_string(trace_count));
        isPayloadTrace = true;
        visitData_part(ctx->data_part());
        isPayloadTrace = false;
        event_count = 0;
        for (const auto& ptr : ctx->event()) {
            visitEvent(ptr);
        }
        event_count = 0;
        tv->exitTrace(traceId);
        trace_count++;
    }
    return {};
}

antlrcpp::Any ServerQueryManager::visitEvent(KnoBABQueryParser::EventContext *ctx) {
    if (ctx) {
        const auto p1 = std::chrono::system_clock::now();
        unsigned long long int timestamp = std::chrono::duration_cast<std::chrono::hours>(p1.time_since_epoch()).count();
        size_t eid = tv->enterEvent(timestamp,ctx->LABEL()->getText());
        isPayloadTrace = false;
        visitData_part(ctx->data_part());
        isPayloadTrace = false;
        tv->exitEvent(eid);
        event_count++;
    }
    return {};
}

antlrcpp::Any ServerQueryManager::visitData_part(KnoBABQueryParser::Data_partContext *ctx) {
    if (ctx && load_also_data) {
        for (const auto& ptr : ctx->field()) {
            visitField(ptr);
        }
    }
    return {};
}


void ServerQueryManager::analyse(const std::string &approach, const DeclareDataAware &clause) {
    auto it = planname_to_declare_to_ltlf.find(approach);
    if (it == planname_to_declare_to_ltlf.end()) {
        throw std::runtime_error(std::string("ERROR: plan name ").append(approach).append(" does not exist"));
    }
    auto it2 = it->second.find(clause.casusu);
    if (it2 == it->second.end()) {
        throw std::runtime_error(std::string("ERROR: plan name ").append(approach).append(" does not implement the specification for the clase ").append(clause.casusu));
    }
}

std::any ServerQueryManager::visitQuery_plan(KnoBABQueryParser::Query_planContext *query_plan) {
    std::string query_plan_name = UNESCAPE(query_plan->STRING()->getText());
    auto& ref = planname_to_declare_to_ltlf[query_plan_name];
    if (!ref.empty()) {
        throw std::runtime_error(std::string("ERROR: the query plan '" + query_plan_name+"' was already declared before with a non-empty number of clauses!"));
    }
    isAutoTimed = query_plan->AUTO_TIMED();
    for (const auto& declare : query_plan->declare_syntax()) {
        auto Q = visitQuery(declare->ltlf());
        std::string declare_name = UNESCAPE(declare->STRING()->getText());
        if (declare->has_args()) {
            if (max_aspect != std::stoull(declare->has_args()->INTNUMBER()->getText())) {
                throw std::runtime_error(std::string("ERROR: declare '").append(declare_name).append("' from query plan '").append(query_plan_name).append("' was associated to ").append(std::to_string(max_aspect)).append(" arguments, but ").append(declare->has_args()->INTNUMBER()->getText()).append(" were expected."));
            }
        }
        ref.emplace(UNESCAPE(declare->STRING()->getText()),
                    visitQuery(declare->ltlf()));
    }
}

LTLfQuery ServerQueryManager::visitQuery(KnoBABQueryParser::LtlfContext *pContext) {
    if (!pContext) return LTLfQuery{}; // if the context is empty, return a query returning empty (the falsehood statement)
    fromNowOnTimed = false;
    max_aspect = DECLARE_TYPE_NONE;
    auto q = std::any_cast<LTLfQuery>(visit(pContext));
    fromNowOnTimed = false;
    return q;
}


antlrcpp::Any ServerQueryManager::visitInit(KnoBABQueryParser::InitContext *context) {
    ASSERT_ON_TIMING(context);
    auto argument = decleare_templates_determine(context->declare_arguments());
    max_aspect = std::max(max_aspect, argument);
    return {LTLfQuery::qINIT(argument, decleare_leaf_determine(context->declare_act_target()), GET_TIMING(context))};
}

antlrcpp::Any ServerQueryManager::visitLast(KnoBABQueryParser::LastContext *context) {
    return {LTLfQuery::qLAST(decleare_leaf_determine(context->declare_act_target()))};
}

antlrcpp::Any ServerQueryManager::visitFirst(KnoBABQueryParser::FirstContext *context) {
    return {LTLfQuery::qFIRST(decleare_leaf_determine(context->declare_act_target()))};
}


antlrcpp::Any ServerQueryManager::visitEnd(KnoBABQueryParser::EndContext *context) {
    ASSERT_ON_TIMING(context);
    auto argument = decleare_templates_determine(context->declare_arguments());
    max_aspect = std::max(max_aspect, argument);
    return {LTLfQuery::qEND(argument, decleare_leaf_determine(context->declare_act_target()), GET_TIMING(context))};
}

antlrcpp::Any ServerQueryManager::visitAbsence(KnoBABQueryParser::AbsenceContext *context) {
    ASSERT_ON_TIMING(context);
    auto argument = decleare_templates_determine(context->declare_arguments());
    max_aspect = std::max(max_aspect, argument);
    return {LTLfQuery::qABSENCE(std::stoull(context->INTNUMBER()->getText()), argument, decleare_leaf_determine(context->declare_act_target()), GET_TIMING(context))};
}

antlrcpp::Any ServerQueryManager::visitExists(KnoBABQueryParser::ExistsContext *context) {
    ASSERT_ON_TIMING(context);
    auto argument = decleare_templates_determine(context->declare_arguments());
    max_aspect = std::max(max_aspect, argument);
    return {LTLfQuery::qEXISTS(std::stoull(context->INTNUMBER()->getText()), argument, decleare_leaf_determine(context->declare_act_target()), GET_TIMING(context), context->NEGATED())};
}

antlrcpp::Any ServerQueryManager::visitNext(KnoBABQueryParser::NextContext *context) {
    fromNowOnTimedStack.push(fromNowOnTimed);
    auto arg = std::any_cast<LTLfQuery>(visit(context->ltlf()));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qNEXT(arg, DEFAULT_TIMING())};
}

antlrcpp::Any ServerQueryManager::visitOr(KnoBABQueryParser::OrContext *context) {
    ASSERT_ON_TIMING(context);
    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qOR(lhs, rhs,
                           GET_TIMING(context),
                           context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitAnd(KnoBABQueryParser::AndContext *context) {
    ASSERT_ON_TIMING(context);
    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qAND(lhs,
                            rhs,
                            GET_TIMING(context),
                            context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitBox(KnoBABQueryParser::BoxContext *context) {

    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto arg = std::any_cast<LTLfQuery>(visit(context->ltlf()));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return {LTLfQuery::qBOX(arg, GET_TIMING(context))};
}

antlrcpp::Any ServerQueryManager::visitDiamond(KnoBABQueryParser::DiamondContext *context) {

    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto arg = std::any_cast<LTLfQuery>(visit(context->ltlf()));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return {LTLfQuery::qDIAMOND(arg, GET_TIMING(context))};
}

antlrcpp::Any ServerQueryManager::visitNot(KnoBABQueryParser::NotContext *context) {

    fromNowOnTimedStack.push(fromNowOnTimed);
    auto arg = std::any_cast<LTLfQuery>(visit(context->ltlf()));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return { LTLfQuery::qNOT(arg,
                             GET_TIMING(context),
                             context->PRESERVE() != nullptr
    )};
}

antlrcpp::Any ServerQueryManager::visitAnd_next_globally(KnoBABQueryParser::And_next_globallyContext *context) {
    ASSERT_ON_GIVEN_TIMING(true);

    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return {LTLfQuery::qANDNEXTGLOBALLY(lhs, rhs, true, context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitAnd_future(KnoBABQueryParser::And_futureContext *context) {
    ASSERT_ON_GIVEN_TIMING(true);

    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return {LTLfQuery::qANDFUTURE(lhs, rhs, true, context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitParen(KnoBABQueryParser::ParenContext *context) {
    return visit(context->ltlf());
}

antlrcpp::Any ServerQueryManager::visitImplication(KnoBABQueryParser::ImplicationContext *context) {
    ASSERT_ON_TIMING(context);
    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qIMPLICATION(lhs,
                                    rhs,
                                    GET_TIMING(context),
                                    context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitUntil(KnoBABQueryParser::UntilContext *context) {
    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qUNTIL(lhs,
                              rhs,
                              GET_TIMING(context),
                              context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitIfte(KnoBABQueryParser::IfteContext *context) {
    ASSERT_ON_TIMING(context);
    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    auto mhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(2)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();
    return {LTLfQuery::qIFTE(lhs,
                             mhs,
                             rhs,
                             GET_TIMING(context),
                             context->THETA() != nullptr)};
}

antlrcpp::Any ServerQueryManager::visitAnd_globally(KnoBABQueryParser::And_globallyContext *context) {
    ASSERT_ON_GIVEN_TIMING(true);

    fromNowOnTimedStack.push(fromNowOnTimed);
    auto lhs = std::any_cast<LTLfQuery>(visit(context->ltlf(0)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    fromNowOnTimedStack.push(fromNowOnTimed);
    fromNowOnTimed = true;
    auto rhs = std::any_cast<LTLfQuery>(visit(context->ltlf(1)));
    fromNowOnTimed = fromNowOnTimedStack.top();
    fromNowOnTimedStack.pop();

    return {LTLfQuery::qANDGLOBALLY(lhs, rhs, true, context->THETA() != nullptr)};
}

#include <httplib.h>
#include <sstream>
#include <nlohmann/json.hpp>

void ServerQueryManager::run(const std::string& host, int port) {
    using namespace httplib;
    Server svr;
    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });
    svr.Post("/query",
             [&](const Request &req, Response &res, const ContentReader &content_reader) {
                 if (req.is_multipart_form_data()) {
                     // NOTE: `content_reader` is blocking until every form data field is read
                     MultipartFormDataItems files;
                     nlohmann::json data;
                     content_reader(
                             [&](const MultipartFormData &file) {
                                 files.push_back(file);
                                 return true;
                             },
                             [&](const char *data, size_t data_length) {
                                 files.back().content.append(data, data_length);
                                 return true;
                             });
                     for (const auto& query : files) {
                         nlohmann::json query_out;
                         const std::string& body = query.content;
                         auto cp = runQuery(body);
                         query_out["result"] = cp.first;
                         query_out["result-type"] = cp.second;
                         data[query.name+"-"+query.filename] = query_out;
                     }
                     res.set_content(data.dump(), "text/json");
                 } else {
                     std::string body;
                     content_reader([&](const char *data, size_t data_length) {
                         body.append(data, data_length);
                         return true;
                     });
                     auto cp = runQuery(body);
                     res.set_content(cp.first, cp.second.c_str());
                 }
             });
    svr.listen(host.c_str(), port);
}

#include <knobab/server/query_manager/KnoBABQueryParser.h>
#include <knobab/server/query_manager/KnoBABQueryLexer.h>
#include <magic_enum.hpp>

std::pair<std::string,std::string> ServerQueryManager::runQuery(const std::string &query) {
    antlr4::ANTLRInputStream input(query);
    KnoBABQueryLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    KnoBABQueryParser parser(&tokens);
    format.clear();
    content.str(std::string());
    parser.queries()->children[0]->accept(this);
    return {content.str(), format};
}

std::any ServerQueryManager::visitDisplay(KnoBABQueryParser::DisplayContext *ctx) {
    if (ctx) {
        std::string env;
        std::string attr;
        int i = 0;
        if (ctx->ACT_TABLE()) {
            i = 1;
            env = UNESCAPE(ctx->STRING(0)->getText());
        } else if (ctx->CNT_TABLE()) {
            i = 2;
            env = UNESCAPE(ctx->STRING(0)->getText());
        } else if (ctx->ATT_TABLE()) {
            i = 3;
            attr = UNESCAPE(ctx->STRING(0)->getText());
            env = UNESCAPE(ctx->STRING(1)->getText());
        }
        auto it = multiple_logs.find(env);
        if (it != multiple_logs.end()) {
            if (i == 1) {
                it->second.print_act_table(content);
            } else if (i == 2) {
                it->second.print_count_table(content);
            } else if (i == 3) {
                auto& ref = it->second.db.attribute_name_to_table;
                auto it2 = ref.find(attr);
                if (it2 != ref.end()) {
                    content << it2->second;
                }
            }
        }
    }
    format = "text/csv";
    return {};
}

std::any ServerQueryManager::visitList(KnoBABQueryParser::ListContext *ctx) {
    if (ctx) {
        std::string env;
        env = UNESCAPE(ctx->STRING()->getText());
        auto it = multiple_logs.find(env);
        if (it != multiple_logs.end()) {
            if (ctx->ATT()) {
                for (auto itx = it->second.db.attribute_name_to_table.begin(), en = it->second.db.attribute_name_to_table.end(); itx != en; ) {
                    content << itx->first;
                    itx++;
                    if (itx != en) content << std::endl;
                }
            } else if (ctx->ACTIVITYLABEL()) {
                for (auto itx =
                        it->second.db.event_label_mapper.int_to_T.begin(),
                        en = it->second.db.event_label_mapper.int_to_T.end(); itx != en; ) {
                    content << *itx;
                    itx++;
                    if (itx != en) content << std::endl;
                }
            }
        }
    }
    format  = "text/csv";
    return {};
}

std::any ServerQueryManager::visitSet_benchmarking_file(KnoBABQueryParser::Set_benchmarking_fileContext *ctx) {
    if (ctx) {
        std::filesystem::path path{UNESCAPE(ctx->file->getText())};
        bool filePreexists = std::filesystem::exists(path);
        std::ofstream log(path, std::ios_base::app | std::ios_base::out);
        if (!filePreexists) {
            LoggerInformation::log_csv_file_header(log);
        }
        if (!log) {
            content << "Error on opening the file";
        } else {
            try {
                for (const auto& ref : infos) {
                    log << std::endl;
                    ref.log_csv_file(log);
                }
                content << "File correctly dumped";
            } catch (...) {
                content << "Error on opening the file";
            }
        }
    }
    format  = "text/plain";
    return {};
}

std::any ServerQueryManager::visitModel_query(KnoBABQueryParser::Model_queryContext *ctx) {
    bool none = true;
    if (ctx) {
        auto env = UNESCAPE(ctx->ensemble->getText());
        auto it = multiple_logs.find(env);
        if (it != multiple_logs.end()) {
            auto plan = UNESCAPE(ctx->plan->getText());
            auto it2 = planname_to_declare_to_ltlf.find(plan);
            if (it2 != planname_to_declare_to_ltlf.end()) {
                it->second.clearModel(); // initializing the model pipeline

                tmpEnv = &it->second;
                plans = &it2->second;
                visit(ctx->model());
                none = false;

                /// GROUNDING
                bool doPreliminaryFill = true;
                bool ignoreActForAttributes = false;
                bool creamOffSingleValues = true;
                GroundingStrategyConf::pruning_strategy grounding_strategy = GroundingStrategyConf::NO_EXPANSION;
                if (ctx->grounding()) {
                    auto ground = ctx->grounding();
                    if (ground->no_preliminary_fill()) doPreliminaryFill = false;
                    if (ground->act_for_attributes()) ignoreActForAttributes = true;
                    if (ground->no_cream_off()) creamOffSingleValues = false;
                    if (ground->strategy) {
                        grounding_strategy = magic_enum::enum_cast<GroundingStrategyConf::pruning_strategy>(UNESCAPE(ground->strategy->getText())).value_or(grounding_strategy);
                    }
                }
                it->second.set_grounding_parameters(doPreliminaryFill, ignoreActForAttributes, creamOffSingleValues, grounding_strategy);
                it->second.doGrounding();

                /// ATOMIZATION
                std::string atomj{"p"};
                AtomizationStrategy atom_strategy = AtomizationStrategy::AtomizeOnlyOnDataPredicates;
                size_t n = 3;
                if (ctx->atomization()) {
                    auto atom = ctx->atomization();
                    if (atom->label) atomj = UNESCAPE(atom->label->getText());
                    if (atom->strlen) n = std::stoull(atom->strlen->getText());
                    if (atom->strategy) atom_strategy = magic_enum::enum_cast<AtomizationStrategy>(UNESCAPE(atom->strategy->getText())).value_or(atom_strategy);
                }
                it->second.set_atomization_parameters(atomj, n , atom_strategy);
                it->second.init_atomize_tables();
                it->second.first_atomize_model();

                /// Init Query
                size_t nThreads = 1;
                EnsembleMethods em = EnsembleMethods::TraceIntersection;
                em = magic_enum::enum_cast<EnsembleMethods>(UNESCAPE(ctx->ensemble->getText())).value_or(em);
                OperatorQueryPlan op = AbidingLogic;
                if (ctx->nothreads) {
                    nThreads = std::stoull(ctx->nothreads->getText());
                }
                if (ctx->operators) {
                    op = magic_enum::enum_cast<OperatorQueryPlan>(UNESCAPE(ctx->operators->getText())).value_or(op);
                }
                it->second.set_maxsat_parameters(nThreads, em, op);
                auto ref = it->second.query_model();

                nlohmann::json result;
                if (ctx->display_qp())
                    result["query_plan"] = ref.generateJSONGraph();
                it->second.experiment_logger.log_json_file(result["benchmark"]);
                switch (ref.final_ensemble) {
                    case PerDeclareSupport:
                        for (size_t i = 0; i < ref.support_per_declare.size(); i++) {
                            result["PerDeclareSupport"][i] = ref.support_per_declare.at(i);
                        }
                        break;

                    case TraceMaximumSatisfiability:
                        for (size_t i = 0; i < ref.max_sat_per_trace.size(); i++) {
                            result["TraceMaximumSatisfiability"][i] = ref.max_sat_per_trace.at(i);
                        }
                        break;

                    case TraceIntersection:
                        for (size_t i = 0, N = ref.result.size(); i<N; i++) {
                            result["TraceIntersection"][i]["trace_id"] = ref.result.at(i).first.first;
                            result["TraceIntersection"][i]["event_id"] = ref.result.at(i).first.second;
                            result["TraceIntersection"][i]["probability"] = ref.result.at(i).second.first;
                            std::vector<std::string> ATC;
                            std::stringstream s;
                            for (const auto& res : ref.result.at(i).second.second) {
                                s.str(std::string());
                                s << res;
                                ATC.emplace_back(s.str());
                            }
                            result["TraceIntersection"][i]["conditions"] = ATC;
                        }
                        break;
                }
                content << result.dump();
            }
        }
    }
    format  = "text/json";
    if (none) {
        content << "{}";
    }
    return {};
}

std::any ServerQueryManager::visitFile_model(KnoBABQueryParser::File_modelContext *ctx) {
    std::filesystem::path declare_file{UNESCAPE(ctx->STRING()->getText())};
    tmpEnv->load_model(declare_file);
    return {};
}

std::any ServerQueryManager::visitDeclares(KnoBABQueryParser::DeclaresContext *ctx) {
    if (ctx) {
        auto v = std::any_cast<std::vector<DeclareDataAware>>(visitData_aware_declare(ctx->data_aware_declare()));
        tmpEnv->load_model(v.begin(), v.end());
    }
    return {};
}

std::any ServerQueryManager::visitTopn(KnoBABQueryParser::TopnContext *ctx) {
    if (ctx) {
        auto topN = std::stoull(ctx->INTNUMBER()->getText());
        auto template_name = UNESCAPE(ctx->STRING()->getText());
        auto v = tmpEnv->generateTopBinaryClauses(template_name,
                                                  topN,
                                                  "");
        tmpEnv->load_model(v.begin(), v.end(), template_name + std::to_string(topN * topN));
    }
    return {};
}
