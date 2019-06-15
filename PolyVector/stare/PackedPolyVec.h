#pragma once
#include <random>
#include <vector>
#include "classes.h"

template <typename T> class PackedPolyVec {
    using SizeEntry_t = int;

public:
    class Iterator {
    public:
        Iterator(std::byte* ptr) :
            ptr(ptr)
        {}
        Iterator operator++() {
            ptr += *reinterpret_cast<SizeEntry_t*>(ptr - sizeof(SizeEntry_t));
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
    };

    PackedPolyVec() :
        size_(0),
        capacity(1000),
        data(1000)
    {}
    ~PackedPolyVec() {
        for (auto& b : *this) {
            b.~T();
        }
    }

    template <typename U, typename ...Args> void emplace_back(Args&&... args) {
        if (size_ + EntrySize<U>() > capacity) {
            capacity *= 2;
            data.resize(capacity);
        }
        *reinterpret_cast<SizeEntry_t*>(&data[size_]) = EntrySize<U>();
        new (&data[size_ + sizeof(SizeEntry_t)]) U(std::forward<Args>(args)...);
        size_ += EntrySize<U>();
    }
    template<typename U> void push_back(const U& value) {
        emplace_back<U>(value);
    }
    template<typename U> void push_back(U&& value) {
        emplace_back<U>(value);
    }

    void reserve(int newCapacity) {
        capacity = newCapacity;
        data.resize(capacity);
    }

    /*T& operator[](int ind) {
        return *reinterpret_cast<T*>(&data[ind]);
    }
    const T& operator[](int ind) const {
        return *reinterpret_cast<T*>(&data[ind]);
    }*/

    int size() const {
        return size_;
    }

    Iterator begin() {
        return Iterator(data.data() + sizeof(SizeEntry_t));
    }
    Iterator end() {
        return Iterator(data.data() + size_ + sizeof(SizeEntry_t));
    }

private:
    template<typename T> static constexpr int EntrySize() {
        return sizeof(SizeEntry_t) + sizeof(T);
    }

    int size_;
    int capacity;
    std::vector<std::byte> data;
};

void fill(PackedPolyVec<Base>& vec, const int N) {
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
int sum(PackedPolyVec<Base>& vec) {
    int sum = 0;
    for (auto& b : vec) {
        sum += b.getData();
    }
    return sum;
}