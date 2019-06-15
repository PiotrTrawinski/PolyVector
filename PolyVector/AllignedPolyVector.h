#pragma once
#include <random>
#include <vector>
#include "classes.h"
#include "NoInitByte.h"

template <typename T> class AllignedPolyVector {
public:
    class Iterator {
    public:
        Iterator(NoInitByte* ptr, int allignment) :
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
        NoInitByte* ptr;
        int allignment;
    };

    AllignedPolyVector(int allignment) :
        allignment_(allignment),
        size_(0),
        data_(50 * allignment)
    {}
    ~AllignedPolyVector() {
        for (auto& b : *this) {
            b.~T();
        }
    }

    template <typename U, typename ...Args> void emplace_back(Args&&... args) {
        if (size_*allignment_ >= data_.size()) {
            data_.resize(data_.size() * 2);
        }
        new (&data_[size_*allignment_]) U(std::forward<Args>(args)...);
        size_ += 1;
    }
    template<typename U> void push_back(const U& value) {
        emplace_back<U>(value);
    }
    template<typename U> void push_back(U&& value) {
        emplace_back<U>(value);
    }
    void pop_back() {
        reinterpret_cast<T*>(&data_[size_*allignment_])->~T();
        size_ -= 1;
    }
    T& back() {
        return *reinterpret_cast<T*>(&data_[size_*allignment_]);
    }

    void reserve(int newCapacity) {
        data_.resize(newCapacity * allignment_);
    }

    T& operator[](int ind) {
        return *reinterpret_cast<T*>(&data_[ind * allignment_]);
    }
    const T& operator[](int ind) const {
        return *reinterpret_cast<T*>(&data_[ind * allignment_]);
    }

    NoInitByte* data() const {
        return data_.data();
    }

    int size() const {
        return size_;
    }

    int capacity() const {
        return static_cast<int>(data_.capacity());
    }

    int allignment() {
        return allignment_;
    }

    Iterator begin() {
        return Iterator(data_.data(), allignment_);
    }
    Iterator end() {
        return Iterator(data_.data() + size_ * allignment_, allignment_);
    }

private:
    int allignment_;
    int size_;
    std::vector<NoInitByte> data_;
};

template <typename T> int maxTypeSize() {
    return sizeof(T);
}
template <typename T, typename U, typename... Rest> int maxTypeSize() {
    return std::max<int>(maxTypeSize<T>(), maxTypeSize<U, Rest...>());
}