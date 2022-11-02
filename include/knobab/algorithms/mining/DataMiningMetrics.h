//
// Created by giacomo on 31/01/2020.
//

#ifndef TUTORIALS_DATAMININGMETRIC_H
#define TUTORIALS_DATAMININGMETRIC_H

#include <map>
#include <vector>
#include <string>
#include <set>

#include <knobab/algorithms/mining/Rule.h>
#include <yaucl/functional/LexicographicalOrder.h>
#include <knobab/dataStructures/CountTableFPTree.h>


using VTLexic = LexicographicalOrder<std::vector<std::string>, std::string>;

/**
 * This struct stores the association itemset/number_of_occurrences returned from the FPGrowth algorithm and returns
 * a set of data mining metrics
 */
struct DataMiningMetrics {

    std::vector<std::vector<trace_t>> act_to_traces;
//    std::map<std::vector<T>, unsigned long, LexicographicalOrder<std::vector<T>, T>> f;///<  Storing the item-support information from the FPGrowth algorithm
    double sumAll = 0.0;                                         ///<  Sum of all the supports for |T|

    /**
     * Initialization via the output of the FPGrowth algorithm
     * @param S
     */
    DataMiningMetrics(const CountTemplate& S) : sumAll{(double)S.nTraces()}, act_to_traces(S.nAct()) {
        for (const auto& ref : S.table) {
            if (ref.id.parts.event_id > 0)
                act_to_traces[ref.id.parts.act].emplace_back(ref.id.parts.trace_id);
        }
    }

    /**
     * Support definition for each item
     * @param i
     * @return
     */
    size_t support(const std::vector<act_t>& i) const {
        if (i.empty())
            return sumAll;
        else if (i.size() == 1)
            return act_to_traces.at(i.at(0)).size();
        else {
            std::vector<trace_t> orig = act_to_traces[i.at(0)];
            std::vector<trace_t> res;
            for (size_t j = 1; j<i.size(); j++) {
                const auto& ref = act_to_traces.at(i.at(j));
                std::set_union(orig.begin(), orig.end(), ref.begin(), ref.end(),
                               std::back_inserter(res));
                std::swap(res, orig);
            }
            return orig.size();
        }
    }

    size_t and_(const std::vector<act_t>& i) const {
        if (i.empty())
            return sumAll;
        else if (i.size() == 1)
            return act_to_traces.at(i.at(0)).size();
        else {
            std::vector<trace_t> orig = act_to_traces[i.at(0)];
            std::vector<trace_t> res;
            for (size_t j = 1; j<i.size(); j++) {
                const auto& ref = act_to_traces.at(i.at(j));
                std::set_intersection(orig.begin(), orig.end(), ref.begin(), ref.end(),
                               std::back_inserter(res));
                std::swap(res, orig);
            }
            return orig.size();
        }
    }

    /**
     * Rule support
     * @param r
     * @return
     */
    double support(const Rule<act_t>& r) const {
        std::vector<act_t> unione;
        for (const auto& x: r.head) unione.emplace_back(x);
        for (const auto& x: r.tail) unione.emplace_back(x);
        std::sort(unione.begin(), unione.end());
        unione.erase(std::unique(unione.begin(), unione.end()), unione.end());
        return ((double)support(unione)) / sumAll;
    }

    /**
     * Rule confidence
     * @param r
     * @return
     */
    double confidence(const Rule<act_t>& r) const {
        std::vector<act_t> unione;
        for (const auto& x: r.head) unione.emplace_back(x);
        for (const auto& x: r.tail) unione.emplace_back(x);
        std::sort(unione.begin(), unione.end());
        unione.erase(std::unique(unione.begin(), unione.end()), unione.end());
        return ((double)and_(unione)) / ((double) support(r.head));
    }

    double lift(const Rule<act_t>& r) const {
        std::vector<act_t> unione;
        for (const auto& x: r.head) unione.emplace_back(x);
        for (const auto& x: r.tail) unione.emplace_back(x);
        std::sort(unione.begin(), unione.end());
        unione.erase(std::unique(unione.begin(), unione.end()), unione.end());
        double pup = ((double)support(unione))/sumAll;
        double ppred = ((double)support(r.head) / sumAll);
        double psucc = ((double)support(r.tail) / sumAll);
        double final = (pup)/(ppred*psucc);
        return final;
    }
};

#endif //TUTORIALS_DATAMININGMETRIC_H
