//
// Created by giacomo on 10/11/22.
//

#ifndef KNOBAB_SERVER_QUERYBUILDER_H
#define KNOBAB_SERVER_QUERYBUILDER_H

#include <ostream>
#include "yaucl/bpm/structures/log/data_loader.h"


std::ostream& load_data_query_from_file(std::ostream& os, log_data_format& file_format, const std::string& filename, bool with_data, bool no_stats, bool with_missing, const std::string& db_name) ;


#endif //KNOBAB_SERVER_QUERYBUILDER_H
