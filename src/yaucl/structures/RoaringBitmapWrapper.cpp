//
// Created by giacomo on 08/12/22.
//

#include "yaucl/structures/RoaringBitmapWrapper.h"


void RoaringBitmapWrapper::addRange(size_t incL, size_t incU) {
    map.addRangeClosed(incL, incU);
}

bool RoaringBitmapWrapper::operator==(const RoaringBitmapWrapper &rhs) const {
    return map == rhs.map;
}

bool RoaringBitmapWrapper::operator!=(const RoaringBitmapWrapper &rhs) const {
    return !(rhs == *this);
}

bool RoaringBitmapWrapper::operator<(const RoaringBitmapWrapper &rhs) const {
    return map.isStrictSubset(rhs.map);
}

bool RoaringBitmapWrapper::operator<=(const RoaringBitmapWrapper &rhs) const {
    return map.isSubset(rhs.map);
}

bool RoaringBitmapWrapper::operator>(const RoaringBitmapWrapper &rhs) const {
    return rhs.map.isStrictSubset(map);
}

bool RoaringBitmapWrapper::operator>=(const RoaringBitmapWrapper &rhs) const {
    return rhs.map.isSubset(map);
}

void RoaringBitmapWrapper::add(size_t x) {
    map.add(x);
}

bool RoaringBitmapWrapper::contains(size_t x) const {
    return map.contains(x);
}

size_t RoaringBitmapWrapper::size() const { return map.cardinality(); }

RoaringBitmapWrapper &RoaringBitmapWrapper::operator&=(const RoaringBitmapWrapper &rhs) {
    map &= rhs.map;
    return *this;
}

RoaringBitmapWrapper &RoaringBitmapWrapper::operator|=(const RoaringBitmapWrapper &rhs) {
    map |= rhs.map;
    return *this;
}

RoaringBitmapWrapper &RoaringBitmapWrapper::operator&=(const roaring::Roaring64Map &rhs) {
    map &= rhs;
    return *this;
}

RoaringBitmapWrapper &RoaringBitmapWrapper::operator|=(const roaring::Roaring64Map &rhs) {
    map |= rhs;
    return *this;
}

RoaringBitmapWrapper &RoaringBitmapWrapper::operator=(const roaring::Roaring64Map &x) {
    map = x;
    return *this;
}

RoaringBitmapWrapper &RoaringBitmapWrapper::operator=(roaring::Roaring64Map &&x) {
    map = x;
    return *this;
}

RoaringBitmapWrapper::RoaringBitmapWrapper(const roaring::Roaring64Map &rhs) : map{rhs} {}

RoaringBitmapWrapper::RoaringBitmapWrapper(roaring::Roaring64Map &&rhs) : map{rhs} {}

RoaringBitmapWrapper::RoaringBitmapWrapper(size_t singleton) : RoaringBitmapWrapper() {
    add(singleton);
}

RoaringBitmapWrapper::RoaringBitmapWrapper(size_t min, size_t max) : RoaringBitmapWrapper() {
    map.addRange(min, max);
}

const roaring::Roaring64MapSetBitForwardIterator RoaringBitmapWrapper::begin() const {
    return map.begin();
}

const roaring::Roaring64MapSetBitForwardIterator RoaringBitmapWrapper::end() const {
    return map.end();
}

#include <yaucl/hashing/hash_combine.h>

size_t RoaringBitmapWrapper::hashCode() const {
    size_t N = map.cardinality();
    uint64_t* ans = new uint64_t[N];
    memset(ans, 0, sizeof(uint64_t)*N);
    map.toUint64Array(ans);
    size_t seed = 13;
    for (size_t i = 0; i<N; i++)
        seed = yaucl::hashing::combine(seed, ans[i]);
    delete ans;
    return seed;
}

std::vector<size_t> RoaringBitmapWrapper::asVector() const {
    std::vector<size_t> n;
    n.reserve(map.cardinality());
    for (size_t i : map) n.emplace_back(i);
    return n;
}

void RoaringBitmapWrapper::clear() {
    map.clear();
}
