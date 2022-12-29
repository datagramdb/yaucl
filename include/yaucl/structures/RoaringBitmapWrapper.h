//
// Created by giacomo on 08/12/22.
//

#ifndef AIRLINE_ROARINGBITMAPWRAPPER_H
#define AIRLINE_ROARINGBITMAPWRAPPER_H

#include <roaring64map.hh>
#include "default_constructors.h"

class RoaringBitmapWrapper {
    roaring::Roaring64Map map;
public:
    DEFAULT_CONSTRUCTORS(RoaringBitmapWrapper);
    RoaringBitmapWrapper(size_t singleton);
    RoaringBitmapWrapper(size_t min, size_t max);
    RoaringBitmapWrapper(const roaring::Roaring64Map &rhs);
    RoaringBitmapWrapper(roaring::Roaring64Map &&rhs);
    RoaringBitmapWrapper& operator=(const roaring::Roaring64Map& x);
    RoaringBitmapWrapper &operator=(roaring::Roaring64Map &&x);
    void add(size_t x);
    void addRange(size_t incL, size_t incU);
    bool contains(size_t x) const;
    size_t size() const;
    const roaring::Roaring64Map& get() const { return map; }
    bool operator==(const RoaringBitmapWrapper &rhs) const;
    bool operator!=(const RoaringBitmapWrapper &rhs) const;
    bool operator<(const RoaringBitmapWrapper& rhs) const;
    bool operator<=(const RoaringBitmapWrapper& rhs) const;
    bool operator>(const RoaringBitmapWrapper& rhs) const;
    bool operator>=(const RoaringBitmapWrapper& rhs) const;
    RoaringBitmapWrapper& operator&=(const roaring::Roaring64Map &rhs);
    RoaringBitmapWrapper& operator|=(const roaring::Roaring64Map &rhs);
    RoaringBitmapWrapper& operator&=(const RoaringBitmapWrapper &rhs);
    RoaringBitmapWrapper& operator|=(const RoaringBitmapWrapper &rhs);
    const roaring::Roaring64MapSetBitForwardIterator begin() const;
    const roaring::Roaring64MapSetBitForwardIterator end() const;
    std::vector<size_t> asVector() const;
    size_t hashCode() const;
};

namespace std {
    template <> struct hash<RoaringBitmapWrapper> {
        size_t operator()(const RoaringBitmapWrapper& x) const {
            return x.hashCode();
        }
    };
}

#endif //AIRLINE_ROARINGBITMAPWRAPPER_H
