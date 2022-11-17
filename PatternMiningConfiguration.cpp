//
// Created by giacomo on 10/11/22.
//

#include "PatternMiningConfiguration.h"

InputFormat::InputFormat(const struct PatternMiningConfiguration& x, const std::string& path) {
    type = x.type;
    loadData = x.loadData;
    this->path = path;
    doPreliminaryFill = x.doPreliminaryFill;
    ignoreActForAttributes = x.ignoreActForAttributes;
    creamOffSingleValues = x.creamOffSingleValues;
    ps = x.ps;
    fresh_atom_label = x.fresh_atom_label;
    setMaximumStrLen = x.setMaximumStrLen;
    mslength = x.mslength;
    strategy = x.strategy;
}

void InputFormat::openFile(std::ifstream& fileOpenAsPath) const {
    if (fileOpenAsPath.is_open())
        fileOpenAsPath.close();
    fileOpenAsPath.open(path);
}

double InputFormat::openEnvironment(Environment& env) const {
    env.doStats = false;
    env.set_grounding_parameters(doPreliminaryFill, ignoreActForAttributes, creamOffSingleValues, ps);
    env.set_atomization_parameters(fresh_atom_label, mslength, strategy);
    double loading_time;
    {
        std::ifstream if_;
        openFile(if_);
        auto t1 = std::chrono::high_resolution_clock::now();
        env.load_log(type, loadData, path, setMaximumStrLen, if_);
        std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - t1;
        loading_time = ms_double.count();
        std::cout << ms_double.count() << "ms (loading)" << std::endl;
    }
    return loading_time;
}

void PatternMiningConfiguration::set_grounding_parameters(bool doPreliminaryFill, bool ignoreActForAttributes, bool creamOffSingleValues, GroundingStrategyConf::pruning_strategy strategy) {
    this->doPreliminaryFill = doPreliminaryFill;
    this->ignoreActForAttributes = ignoreActForAttributes;
    this->creamOffSingleValues = creamOffSingleValues;
    ps = strategy;
}

void PatternMiningConfiguration::set_atomization_parameters(const std::string &fresh_atom_label, size_t mslength, AtomizationStrategy strategy) {
    this->fresh_atom_label = fresh_atom_label;
    this->mslength = mslength;
    this->strategy = strategy;
}
