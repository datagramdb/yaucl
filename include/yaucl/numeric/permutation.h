//
// Created by giacomo on 28/01/2020.
//

#ifndef TUTORIALS_PERMUTATION_H
#define TUTORIALS_PERMUTATION_H

#include <vector>
#include <ctime>
#include <cstdlib>

namespace yaucl {
    namespace numeric {
        // Function to return the next random number
        int getNum(std::vector<int>& v);

        // Function to generate n non-repeating random numbers
        std::vector<int> generateRandomPermutation(int n);

        template <typename T> std::vector<T> permute(const std::vector<T>& toPermute, const std::vector<int>& permutation) {
            std::vector<T> toReturn;
            for (const int& x : permutation)
                toReturn.emplace_back(toPermute[x]);
            return toReturn;
        }
    }
}



#endif //TUTORIALS_PERMUTATION_H
