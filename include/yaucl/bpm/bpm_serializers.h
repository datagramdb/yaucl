//
// Created by giacomo on 25/10/21.
//

#ifndef YAUCL_BPM_SERIALIZERS_H
#define YAUCL_BPM_SERIALIZERS_H

#include <yaucl/bpm/bpm_log.h>
#include <ostream>

namespace yaucl {
    namespace bpm {
        void serialize_non_data_log(const log& simple_log, std::ostream& xes_file);
        void serialize_data_log(const log& payload_log, std::ostream& xes_file);

        log load_xes_ignore_data(const std::string &filename, bool conservative_for_pddl = true);
        log load_xes_with_data(const std::string &filename, bool conservative_for_pddl = true);
    }
}

#endif //YAUCL_BPM_SERIALIZERS_H
