//
// Created by giacomo on 02/10/24.
//

#include "dt/structures/Nodes.h"

Nodes::Nodes(size_t begin, size_t end, size_t max_height): begin{begin}, end{end}, rightOffset{0}, split{0}, isLeaf{false}, majority_class{-1}, majority_class_precision{0.0}, max_height{max_height} {
candidate.second = -1;
}