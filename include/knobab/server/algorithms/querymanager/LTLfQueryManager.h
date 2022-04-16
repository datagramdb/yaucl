//
// Created by giacomo on 16/04/2022.
//

#ifndef KNOBAB_SERVER_LTLFQUERYMANAGER_H
#define KNOBAB_SERVER_LTLFQUERYMANAGER_H


#include <yaucl/numeric/ssize_t.h>
#include <knobab/server/algorithms/atomization/AtomizingPipeline.h>
#include <unordered_map>
#include <knobab/server/operators/LTLfQuery.h>


struct LTLfQueryManager {
    std::unordered_map<LTLfQuery, LTLfQuery*> conversion_map_for_subexpressions;
    std::map<size_t, std::vector<LTLfQuery*>> Q;
    std::unordered_map<LTLfQuery*, size_t> counter;
    std::vector<LTLfQuery*> atomsToDecomposeInUnion;
    size_t current_query_id = 0;
    std::vector<std::unordered_set<LTLfQuery*>> activations;

    void generateGraph(std::map<LTLfQuery*, std::vector<LTLfQuery*>>& ref, LTLfQuery*q) const;
    std::string generateGraph() const;

    /**
     * Clears the manager, so to get ready with another query plan to be analysed
     */
    void clear();


    LTLfQuery *simplify(const std::unordered_set<std::string>& atom,
                        size_t formulaId,
                        const LTLfQuery &input,
                        const DeclareDataAware *joinCondition,
                        const std::unordered_set<std::string> &atom_universe,
                        const std::unordered_set<std::string> &left,
                        const std::unordered_set<std::string> &right,
                        std::vector<std::string> &toUseAtoms,
                        std::unordered_map<std::string , std::vector<size_t>>& atomToFormulaId);

    void finalize_unions(const AtomizingPipeline& ap, std::vector<LTLfQuery*>& W, KnowledgeBase* ptr);

private:

    LTLfQuery* simplify(const LTLfQuery& q);
    LTLfQuery* simplifyRecursively(LTLfQuery &element_disjunction);
};



#endif //KNOBAB_SERVER_LTLFQUERYMANAGER_H
