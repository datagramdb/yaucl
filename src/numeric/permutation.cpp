//
// Created by giacomo on 28/01/2020.
//

#include <yaucl/numeric/permutation.h>

int yaucl::numeric::getNum(std::vector<int> &v) {
    // Size of the vector
    int n = v.size();

    // Generate a random number
    srand(time(NULL));

    // Make sure the number is within
    // the index range
    int index = rand() % n;

    // Get random number from the vector
    int num = v[index];

    // Remove the number from the vector
    std::swap(v[index], v[n - 1]);
    v.pop_back();

    // Return the removed number
    return num;
}

std::vector<int> yaucl::numeric::generateRandomPermutation(int n) {
    std::vector<int> v(n);
    std::vector<int> output;

    // Fill the vector with the values
    // 1, 2, 3, ..., n
    for (int i = 0; i < n; i++)
        v[i] = i + 1;

    // While vector has elements
    // get a random number from the vector and print it
    while (v.size()) {
        output.emplace_back(getNum(v)-1);
    }
    return output;
}
