//
// Created by giacomo on 26/07/21.
//

#ifndef BPM21_TEMPLATECOLLECTRESULT_H
#define BPM21_TEMPLATECOLLECTRESULT_H

#include <vector>
#include <yaucl/graphs/graph_join_pm.h>

struct TemplateCollectResult {
    std::vector<graph_join_pm> distinct_graph_model;
    graph_join_pm              joined_graph_model;

    TemplateCollectResult() = default;
    TemplateCollectResult(const TemplateCollectResult& ) = default;
    TemplateCollectResult(TemplateCollectResult&& ) = default;
    TemplateCollectResult& operator=(const TemplateCollectResult& ) = default;
    TemplateCollectResult& operator=(TemplateCollectResult&& ) = default;

    [[deprecated]]
    void minimize();

};

void conditionalPruningGraph(bool doPrune, bool firstInsertion, TemplateCollectResult& result, graph_join_pm& currGraph);


#endif //BPM21_TEMPLATECOLLECTRESULT_H
