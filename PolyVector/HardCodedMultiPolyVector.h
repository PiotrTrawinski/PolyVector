#pragma once
#include <vector>
#include <tuple>
#include <unordered_map>
#include "classes.h"

class HardCodedMultiPolyVector {
public:
    HardCodedMultiPolyVector() {}
    ~HardCodedMultiPolyVector() {}

    template<typename U, typename... Args> void emplace_back(Args&&... args) {
        if constexpr (std::is_same<U, Base>::value) {
            baseVec.emplace_back(args...);
        }
        if constexpr (std::is_same<U, Derived>::value) {
            derivedVec.emplace_back(args...);
        }
        if constexpr (std::is_same<U, Derived2>::value) {
            derived2Vec.emplace_back(args...);
        }
        if constexpr (std::is_same<U, Derived3>::value) {
            derived3Vec.emplace_back(args...);
        }
    }

    void reserve(int reserveSize) {
        baseVec.reserve(reserveSize);
        derivedVec.reserve(reserveSize);
        derived2Vec.reserve(reserveSize);
        derived3Vec.reserve(reserveSize);
    }

    std::vector<Base> baseVec;
    std::vector<Derived> derivedVec;
    std::vector<Derived2> derived2Vec;
    std::vector<Derived3> derived3Vec;
};