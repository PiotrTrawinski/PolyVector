#pragma once
#include <random>
#include <vector>
#include "classes.h"

template <typename T> class PolyVec {
public:
    class Iterator {
    public:
        Iterator(std::byte* ptr, int allignment) :
            ptr(ptr),
            allignment(allignment)
        {}
        Iterator operator++() {
            ptr += allignment;
            return *this;
        }
        bool operator==(const Iterator& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const Iterator& other) const {
            return ptr != other.ptr;
        }
        T& operator*() const {
            return *reinterpret_cast<T*>(ptr);
        }
        T* operator->() const {
            return reinterpret_cast<T*>(ptr);
        }
    private:
        std::byte* ptr;
        int allignment;
    };

    PolyVec(int allignment) :
        allignment(allignment),
        size_(0),
        capacity(10),
        data(capacity * allignment)
    {}
    ~PolyVec() {
        for (auto& b : *this) {
            b.~T();
        }
    }

    template <typename U, typename ...Args> void emplace_back(Args&&... args) {
        if (size_ >= capacity) {
            capacity *= 2;
            data.resize(capacity * allignment);
        }
        new (&data[size_*allignment]) U(std::forward<Args>(args)...);
        size_ += 1;
    }
    template<typename U> void push_back(const U& value) {
        emplace_back<U>(value);
    }
    template<typename U> void push_back(U&& value) {
        emplace_back<U>(value);
    }

    void reserve(int newCapacity) {
        capacity = newCapacity;
        data.resize(capacity * allignment);
    }

    T& operator[](int ind) {
        return *reinterpret_cast<T*>(&data[ind * allignment]);
    }
    const T& operator[](int ind) const {
        return *reinterpret_cast<T*>(&data[ind * allignment]);
    }

    int size() const {
        return size_;
    }

    Iterator begin() {
        return Iterator(data.data(), allignment);
    }
    Iterator end() {
        return Iterator(data.data() + size_*allignment, allignment);
    }

private:
    int allignment;
    int size_;
    int capacity;
    std::vector<std::byte> data;
};

template <typename T> int maxTypeSize() {
    return sizeof(T);
}
template <typename T, typename U, typename ...Rest> int maxTypeSize() {
    return std::max(maxTypeSize<T>(), maxTypeSize<U, Rest...>());
}

void fill(PolyVec<Base>& vec, const int N) {
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
int sum(PolyVec<Base>& vec) {
    int sum = 0;
    for (auto& b : vec) {
        sum += b.getData();
    }
    return sum;
}