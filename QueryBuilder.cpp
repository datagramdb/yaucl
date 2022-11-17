//
// Created by giacomo on 10/11/22.
//

#include "QueryBuilder.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::ostream& load_data_query_from_file(std::ostream& os, log_data_format& file_format, const std::string& filename, bool with_data, bool no_stats, bool with_missing, const std::string& db_name) {
    //'load' (HRF|TAB|XES|log) file=STRING with_data? no_stats? with_missing? 'as' env_name=STRING
    os << "load ";
    switch (file_format) {
        case HUMAN_READABLE_YAUCL:
            os << "HRF ";
            break;
        case XES1:
            os << "XES ";
            break;
        case TAB_SEPARATED_EVENTS:
            os << "TAB ";
            break;
    }
    os << std::quoted(filename)<< " ";
    if (with_data)
        os << " with data ";
    if (no_stats)
        os << " no stats ";
    if (with_missing)
        os << " with missing ";
    return os << " as " << std::quoted(db_name);
}