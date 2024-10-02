//
// Created by giacomo on 02/10/24.
//

#ifndef DT_FIND_ARGUMENT_H
#define DT_FIND_ARGUMENT_H
#include  <dt/ForTheWin.h>
#include <dt/dt_predicate.h>

struct find_argument {
    static union_minimal a, b, min;
    static std::string nostring;
    std::pair<std::string,union_minimal> wd;

    find_argument() {
        wd.second = min;
    }

    inline const union_minimal& find(const record& map, const std::string& key) {
        int offset = find_offset(map, key);
        if (offset == -1)
            return a;
        else
            return map[offset].second;
    }

    static inline const double findNumeric(const record& map, const int offset) {
        if (offset == -1)
            return 0.0;
        else
            return std::get<double>(map[offset].second);
    }

    static inline const std::string& findString(const record& map, const int offset) {
        if (offset == -1)
            return nostring;
        else
            return std::get<std::string>(map[offset].second);
    }

    inline int find_offset(const record& map, const std::string& key) {
        wd.first = key;
        auto it = std::lower_bound(map.begin(), map.end(), wd);
        if (it == map.end())
            return -1;
        else
            return it-map.begin();
    }
};


#endif //DT_FIND_ARGUMENT_H
