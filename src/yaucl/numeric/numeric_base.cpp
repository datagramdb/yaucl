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
#include <chrono>
#include <iomanip>

long long int yaucl::numeric::parse8601(const std::string &save) {
    std::istringstream in{save};
    date::sys_time<std::chrono::milliseconds> tp;
    in >> date::parse("%FT%TZ", tp);
    if (in.fail())
    try {
        in.clear();
        in.exceptions(std::ios::failbit);
        in.str(save);
        in >> date::parse("%FT%T%Ez", tp);
    } catch(...) {
        try {
            std::tm tm = {};
            std::stringstream ss( save );
            ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%OS6");
            std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(mktime(&tm));
            return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
        } catch(...) {
            return std::chrono::duration_cast< std::chrono::milliseconds >(
                    std::chrono::system_clock::now().time_since_epoch()
            ).count();
        }
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}
