//
// Created by giacomo on 30/04/2022.
//

#include "knobab/server/ndp/KnowledgeBaseNDPLoader.h"
#include "../../../NDPAttributeTable.h"
#include <yaucl/data/MemoryMappingFile.h>
#include <knobab/server/ndp/count_table_rcx.h>
#include <yaucl/data/VariadicSizeArrayElements.h>
#include <knobab/server/ndp/act_table_rcx.h>
#include <knobab/server/ndp/KnowledgeBaseNDPReader.h>
#include <yaucl/data/VarSizeNDPSorter.h>

void external_var_sorter() {
    {
        VarSizeNDPWriter<std::string> W{"/home/giacomo/strings.bin", [](const std::string& w,new_iovec& out) {
            out.iov_len = w.size()+1;
            out.iov_base = (void*)w.data();
        }};
        W.put("hello");
        W.put("wor");
        W.put("ciao");
        W.put("aakward");
        W.put("fo");
    }
    {
        VarSizeNDPReader f{"/home/giacomo/strings.bin"};
        for (size_t i = 0; i<5; i++) {
            std::cout << f.get(i).stream().str() << std::endl;
        }
    }

    VarSizeNDPSorter K{20, [](const new_iovec& l, const new_iovec& r, char* p) {
        std::string left{(char*)p+(size_t)l.iov_base, l.iov_len-1};
        std::string right{(char*)p+(size_t)r.iov_base, r.iov_len-1};
        return left < right;
    }};
    K.sort("/home/giacomo/strings.bin", "/home/giacomo/tmp");
    {
        VarSizeNDPReader f{"/home/giacomo/strings.bin"};
        for (size_t i = 0; i<5; i++) {
            std::cout << f.get(i).stream().str() << std::endl;
        }
    }
}

#include <string>

void external_var_readwriter() {
    VarSizeNDPReaderWriter<std::string> W{"/home/giacomo/strings.bin",
                                          "/home/giacomo/tmp",
                                          [](const std::string& w,new_iovec& out) {
                                            out.iov_len = w.size();
                                            out.iov_base = (void*)w.data();
                                          },
                                          [](const new_iovec& memo) {
                                              return std::string((char*)memo.iov_base, memo.iov_len);
    }};
    W.put("hello");
    W.put("wor");
    W.put("ciao");
    W.put("aakward");
    W.put("fo");
    W.removeByValue({"ciao"});
    W.removeById({0});
    for (size_t i = 0, N = W.size(); i<N; i++) {
        std::cout << W.get(i) << std::endl;
    }
    W.sort(20);
    for (size_t i = 0, N = W.size(); i<N; i++) {
        std::cout << W.get(i) << std::endl;
    }
}

void loader() {
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

    KnowledgeBaseNDPLoader loader{"/home/giacomo/test"};
    loader.reloadFromFiles(reader);


    for (size_t i = 0, N = reader.get_n_activity_labels(); i<N; i++) {
        std::cout << reader.get_ith_activity_label(i) << std::endl;
        {
            auto cp = reader.count_table_scan(i);
            while (cp.first != cp.second) {
                std::cout << "\t" << cp.first->act_id << "@" << cp.first->trace_id << " #" << cp.first->count << std::endl;
                cp.first++;
            }
        }
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        {
            auto cp = reader.act_table_scan(i);
            while (cp.first != cp.second) {
                std::cout << "\t" << cp.first->act_id << "@ (" << cp.first->trace_id << "," << cp.first->event_id << ")" <<  std::endl;
                cp.first++;
            }
        }
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    }

    {
        for (size_t i = 0, N = reader.get_n_traces(); i<N; i++) {
            for (size_t j = 0, M = reader.get_ith_trace_length(i); j<M; j++ ) {
                auto act_trace = reader.get_event_act(i, j);
                std::cout << act_trace->act_id << ", " << act_trace->trace_id << "@" << act_trace->event_id << std::endl;
            }
        }
    }
}

#include <yaucl/data/FixedSizeNDPSorter.h>
#include <knobab/server/query_manager/NDPFuzzyStringMatching.h>

void array_updater() {
    yaucl::data::VariadicSizeArrayElementsReaderWriter updater{"/home/giacomo/updatable.bin"};
    {
        std::string S{"hello"};
        updater.put({(void*)S.c_str(), S.size()+1});
    }
    {
        std::string S{"world"};
        updater.put({(void*)S.c_str(), S.size()+1});
    }
    {
        std::string S{"foca"};
        updater.update(1, {(void*)S.c_str(), S.size()+1});
    }
    size_t N = updater.size();
    for (size_t i = 0; i<N; i++) {
        std::cout << updater.get<char>(i) << std::endl;
    }

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    {
        std::string S{"ciao"};
        updater.update(0, {(void*)S.c_str(), S.size()+1});
    }
    {
        std::string S{"monaca"};
        updater.put({(void*)S.c_str(), S.size()+1});
    }
    N = updater.size();
    for (size_t i = 0; i<N; i++) {
        std::cout << updater.get<char>(i) << std::endl;
    }
}

void fixed_size() {
    FixedSizeReaderWriter<size_t> W{"/home/giacomo/numbers.bin", "/home/giacomo/tmp"};
    W.put(10);
    W.put(2);
    W.put(5);
    W.put(3);
    W.put(18);
    for (size_t i = 0, N = W.size(); i<N; i++) {
        std::cout << W.get(i) << std::endl;
    }
    W.sort(sizeof(size_t)*2);
    for (size_t i = 0, N = W.size(); i<N; i++) {
        std::cout << W.get(i) << std::endl;
    }
}

void string_fuzzy_matching() {
    NDPFuzzyStringMatching matching{"fuzzyStringTest"};
    matching.put("hello");
matching.put("cello");
    matching.put("hell");
    matching.put("bell");
    matching.put("cellulare");
    matching.put("organo");
    std::multimap<double, std::string> result;
    matching.fuzzyMatch(0.0, 100, "organello", result);
    for (const auto& cp : result)
        std::cout << cp << std::endl;
}


void attribute_table() {
    NDPAttributeTable test{"testing", "x", SizeTAtt};
    test.record_load(10, {10ULL}, 0);
    test.record_load(2, {20ULL}, 1);
    test.record_load(2, {30ULL}, 2);
    test.record_load(2, {20ULL}, 3);
    test.record_load(5, {40ULL}, 4);
    test.index();
}

void attribute_table_string() {
    NDPAttributeTable test{"testing", "x", StringAtt};
    test.record_load(10, "ciao", 0);
    test.record_load(2, "hello", 1);
    test.record_load(2, "bello", 2);
    test.record_load(2, "wello", 3);
    test.record_load(5, "bullo", 4);
    test.index();
}

int main(void) {
    string_fuzzy_matching();
}