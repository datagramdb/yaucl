//
// Created by giacomo on 03/01/21.
//

#ifndef CUCCIOLO_REMOVEINDICESFROMVECTOR_H
#define CUCCIOLO_REMOVEINDICESFROMVECTOR_H

#include <vector>

/**
 * This utility function removes from a vector v all the elements at the (sorted) positions in [begin,end)
 *
 * @tparam T        type parameter
 * @tparam Iter     index iterator parameter
 * @param v         Vector which elements are to be removed.
 * @param begin     Beginning of sorted indices
 * @param end       End of sorted indices
 */
template <typename T, typename Iter>
void removeIndicesFromVector(std::vector<T>& v, Iter begin, Iter end)
// requires std::is_convertible_v<std::iterator_traits<Iter>::value_type, std::size_t>
{
    ///assert(std::is_sorted(begin, end));
    if (v.empty()) return; // do not allocate and perform extra operations if the vector is empty
    auto rm_iter = begin;
    std::size_t current_index = 0;

    const auto pred = [&](const T&){
        // any more to remove?
        if (rm_iter == end) { return false; }
        // is this one specified?
        if (*rm_iter == current_index++) { return ++rm_iter, true; }
        return false;
    };

    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}


#endif //CUCCIOLO_REMOVEINDICESFROMVECTOR_H
