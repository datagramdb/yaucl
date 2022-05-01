//
// Created by giacomo on 30/04/2022.
//

#include "knobab/server/ndp/KnowledgeBaseNDPLoader.h"
#include <yaucl/data/MemoryMappingFile.h>
#include <knobab/server/ndp/count_table_rcx.h>


int main(void) {
    {
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


}