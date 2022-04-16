//
// Created by giacomo on 25/10/21.
//
#include <yaucl/numeric/numeric_base.h>

std::vector <size_t> yaucl::numeric::numberToBase(size_t n, size_t b) {
    if (n == 0)
        return {0};
    std::vector<size_t> digits;
    while (n) {
        digits.emplace(digits.begin(), n % b);
        n = n/b;
    }
    return digits;
}

#include <date/date.h>

long long int yaucl::numeric::parse8601(const std::string &save) {
    std::istringstream in{save};
    date::sys_time<std::chrono::milliseconds> tp;
    in >> date::parse("%FT%TZ", tp);
    if (in.fail())
    {
        in.clear();
        in.exceptions(std::ios::failbit);
        in.str(save);
        in >> date::parse("%FT%T%Ez", tp);
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}
