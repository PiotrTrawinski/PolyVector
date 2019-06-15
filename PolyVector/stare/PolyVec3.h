#pragma once
#include <vector>
#include <random>
#include <functional>
#include "classes.h"

template<typename... Args> class PolyVec3_;
template<> class PolyVec3_<int> {
public:
    void push_back(const int& value) {}
    void* operator[](int id) { return nullptr; }
    int size() { return 0; }
    void reserve(int capacity) {}
    template<typename U> std::vector<U>& getVec() {
        return std::vector<U>();
    }
};
template<typename T> class PolyVec3Iterator {
public:
    PolyVec3Iterator operator+() {
        current += 1;
        return *this;
    }
private:
    T* current;
};
template<typename T, typename... Args> class PolyVec3_<T, Args...> {
public:
    std::vector<T> data;
    PolyVec3_<Args...> rest;
    template <typename U> void push_back(const U& value) {
        rest.push_back(value);
    }
    template<> void push_back(const T& value) {
        data.push_back(value);
    }
    void* operator[](int ind) {
        if (ind < data.size()) {
            return reinterpret_cast<void*>(&data[ind]);
        } else {
            return reinterpret_cast<void*>(rest[ind - data.size()]);
        }
    }
    int size() {
        return data.size() + rest.size();
    }
    void reserve(int capacity) {
        data.reserve(capacity);
        rest.reserve(capacity);
    }

    template<typename U> std::vector<U>& getVec() {
        return rest.getVec<U>();
    }
    template<> std::vector<T>& getVec() {
        return data;
    }
};

template<typename Base, typename... DerivedTypes> class PolyVec3 {
public:
    template<typename T, typename... Args> void emplace_back(Args... args) {
        vec.push_back<T>(T(args...));
    }
    Base& operator[](int ind) {
        return *reinterpret_cast<Base*>(vec[ind]);
    }
    int size() {
        return vec.size();
    }
    void reserve(int capacity) {
        vec.reserve(capacity);
    }
    template<typename T> std::vector<T>& getVec() {
        return vec.getVec<T>();
    }

    PolyVec3Iterator<Base> begin() {
        return getVec<Base>();
    }
    PolyVec3Iterator<int> end() {
        return getVec<int>();
    }

private:
    PolyVec3_<Base, DerivedTypes..., int> vec;
};

void fill(PolyVec3<Base, Derived, Derived2, Derived3>& vec, const int N) {
    std::mt19937 gen(1234);
    std::uniform_int_distribution<int> dist(0, 100);
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
        case 0: vec.emplace_back<Base>(dist(gen)); break;
        case 1: vec.emplace_back<Derived>(dist(gen), dist(gen)); break;
        case 2: vec.emplace_back<Derived2>(dist(gen), dist(gen)); break;
        case 3: vec.emplace_back<Derived3>(dist(gen), dist(gen)); break;
        }
    }
}
int sum(PolyVec3<Base, Derived, Derived2, Derived3>& vec) {
    int sum = 0;
    auto& baseVec = vec.getVec<Base>();
    auto& derivedVec = vec.getVec<Derived>();
    auto& derived2Vec = vec.getVec<Derived2>();
    auto& derived3Vec = vec.getVec<Derived3>();
    for (int i = 0; i < baseVec.size(); ++i) {
        sum += baseVec[i].getData();
    }
    for (int i = 0; i < derivedVec.size(); ++i) {
        sum += derivedVec[i].getData();
    }
    for (int i = 0; i < derived2Vec.size(); ++i) {
        sum += derived2Vec[i].getData();
    }
    for (int i = 0; i < derived3Vec.size(); ++i) {
        sum += derived3Vec[i].getData();
    }
    /*for (int i = 0; i < vec.size(); ++i) {
        sum += vec[i].getData();
    }*/
    return sum;
}