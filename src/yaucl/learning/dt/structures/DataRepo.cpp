//
// Created by giacomo on 02/10/24.
//

#include "yaucl/learning/dt/structures/DataRepo.h"

DataRepo::DataRepo(const data_record &records, const data_clazzes &records_classes, size_t max_height, double maxPrec,
                   size_t l, size_t eta, ForTheWin::gain_measures measure) : measure{measure}, records{records}, records_classes{records_classes}, l{l}, eta{eta}, maxPrec{maxPrec}, max_height{max_height} {
    max_record_size = std::min(records_classes.size(), records.size());
    offsets.resize(max_record_size);
    fieldOffset.resize(max_record_size, 0);
    for (size_t i = 0; i<max_record_size; i++) {
        offsets[i] = i;
        max_class_id = std::max(max_class_id, records_classes[i]);
    }
    nv.init(max_class_id);
    forthegain.init(max_class_id);
}
