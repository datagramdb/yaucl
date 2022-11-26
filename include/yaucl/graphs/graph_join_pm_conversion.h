/*
 * graph_join_pm_conversion.h
 * This file is part of yaucl
 *
 * Copyright (C) 2022 - Giacomo Bergami
 *
 * yaucl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaucl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaucl. If not, see <http://www.gnu.org/licenses/>.
 */


//
// Created by giacomo on 26/07/21.
//

#ifndef BPM21_GRAPH_JOIN_PM_CONVERSION_H
#define BPM21_GRAPH_JOIN_PM_CONVERSION_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <yaucl/graphs/FlexibleFA.h>
#include <yaucl/graphs/graph_join_pm.h>
//#include <graphs/third-party-wrappers/FLLOATScriptRunner.h>

void convert_to_novel_graph(const FlexibleFA<std::string, size_t> &old, graph_join_pm& result);

FlexibleFA<size_t, std::string> convert_to_dfa_graph(graph_join_pm& input);

/*
void parseToGraph(graph_join_pm& result,
                  ltlf& formulas_to_dfas,
                  FLLOATScriptRunner &pyscript,
                  const std::string single_line_clause_file,
                  std::unordered_map<std::string, std::string>* ptr,
                  const std::unordered_set<std::string>& SigmaAll);
*/

#endif //BPM21_GRAPH_JOIN_PM_CONVERSION_H
