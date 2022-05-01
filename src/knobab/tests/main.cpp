//
// Created by giacomo on 30/04/2022.
//

#include "knobab/server/ndp/KnowledgeBaseNDPLoader.h"
#include <yaucl/data/MemoryMappingFile.h>
#include <knobab/server/ndp/count_table_rcx.h>
#include <yaucl/data/VariadicSizeArrayElements.h>
#include <knobab/server/ndp/act_table_rcx.h>
#include <knobab/server/ndp/KnowledgeBaseNDPReader.h>


int main(void) {
    if (true) {
        KnowledgeBaseNDPLoader loader{"/home/giacomo/test"};
        loader.enterLog("s", "s");
        {
            auto id = loader.enterTrace("A");
            loader.exitEvent(loader.enterEvent(0, "a"));
            loader.exitEvent(loader.enterEvent(1, "c"));
            loader.exitEvent(loader.enterEvent(3, "h"));
            loader.exitEvent(loader.enterEvent(4, "h"));
            loader.exitEvent(loader.enterEvent(5, "b"));
            loader.exitEvent(loader.enterEvent(6, "h"));
            loader.exitTrace(id);
        }
        {
            auto id = loader.enterTrace("B");
            loader.exitEvent(loader.enterEvent(0, "i"));
            loader.exitEvent(loader.enterEvent(1, "b"));
            loader.exitEvent(loader.enterEvent(2, "a"));
            loader.exitEvent(loader.enterEvent(3, "a"));
            loader.exitEvent(loader.enterEvent(4, "b"));
            loader.exitTrace(id);
        }
        loader.exitLog("s", "s");
    }

    KnowledgeBaseNDPReader reader{"/home/giacomo/test"};
    for (size_t i = 0, N = reader.get_n_activity_labels(); i<N; i++) {
        std::cout << reader.get_ith_activity_label(i) << std::endl;
        auto cp = reader.act_table_scan(i);
        while (cp.first != cp.second) {
            std::cout << "\t" << cp.first->act_id << "@" << cp.first->trace_id << " #" << cp.first->count << std::endl;
            cp.first++;
        }
    }

    {
        for (size_t i = 0, N = reader.get_n_traces(); i<N; i++) {
            for (size_t j = 0, M = reader.get_ith_trace_length(i); j<M; j++ ) {
                auto act_trace = reader.get_event_act(i, j);
                std::cout << act_trace->act_id << ", " << act_trace->trace_id << "@" << act_trace->event_id << " offset=" << act_trace->absolute_sequence << std::endl;
            }
        }
    }
}