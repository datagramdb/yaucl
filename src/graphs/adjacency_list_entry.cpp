//
// Created by giacomo on 05/07/21.
//

#include "yaucl/graphs/adjacency_list_entry.h"
using namespace yaucl::graphs;

yaucl::graphs::adjacency_list_entry::adjacency_list_entry(size_t id, const std::vector<std::string> &content,
                                           const std::vector<size_t> &nesting) : id(id), content(content), nesting(nesting) {}

adjacency_list_entry::adjacency_list_entry(size_t id, const std::vector<std::string> &content,
                                           const std::vector<size_t> &nesting,
                                           const std::unordered_map<size_t, edge_collateral> &ingoing,
                                           const std::unordered_map<size_t, edge_collateral> &outgoing,
                                           bool isHierarchyRoot, bool isUpperHierarchy) : id(id), content(content),
                                                                                          nesting(nesting),
                                                                                          ingoing(ingoing),
                                                                                          outgoing(outgoing),
                                                                                          isHierarchyRoot(
                                                                                                  isHierarchyRoot),
                                                                                          isUpperHierarchy(
                                                                                                  isUpperHierarchy) {}
