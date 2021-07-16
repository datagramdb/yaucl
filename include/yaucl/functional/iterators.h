//
// Created by giacomo on 16/07/21.
//

#ifndef YAUCL_ITERATORS_H
#define YAUCL_ITERATORS_H

#include <unordered_map>
#include <map>

namespace yaucl {
    namespace iterators {


        template <typename typemapite, typename lambda>
        void map_iterate_key_intersection(typemapite it_left, typemapite e_left, typemapite it_right, typemapite e_right,  lambda function) {
            while ((it_left != e_left) && (it_right != e_right)) {
                if (it_left->first < it_right->first)
                    it_left++;
                else if (it_left->first > it_right->first)
                    it_right++;
                else {
                    function(it_left, it_right);
                    it_left++;
                    it_right++;
                }
            }
        }

    }
}

#endif //YAUCL_ITERATORS_H
