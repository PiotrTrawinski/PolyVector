#pragma once
#include <random>
#include <vector>
#include "classes.h"
#include "NoInitByte.h"

template <typename T, typename SizeEntry_t=int> class IndexPackedPolyVector {
public:
    class Iterator {
    public:
        Iterator(NoInitByte* ptr) :
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
        NoInitByte* ptr;
    };

    IndexPackedPolyVector() :
        size_(0),
        sizeInBytes_(0),
        data_(1000),
        indexToOffset(50)
    {
        *reinterpret_cast<SizeEntry_t*>(&data_[0]) = 0;
    }
    ~IndexPackedPolyVector() {
        for (auto& b : *this) {
            b.~T();
        }
    }

    template <typename U, typename ...Args> void emplace_back(Args&&... args) {
        sizeInBytes_ += *reinterpret_cast<SizeEntry_t*>(&data_[sizeInBytes_]);
        if (sizeInBytes_ + EntrySize<U>() > data_.size()) {
            data_.resize(data_.size() * 2);
        }
        *reinterpret_cast<SizeEntry_t*>(&data_[sizeInBytes_]) = EntrySize<U>();
        new (&data_[sizeInBytes_ + sizeof(SizeEntry_t)]) U(std::forward<Args>(args)...);
        indexToOffset.emplace_back(&data_[sizeInBytes_ + sizeof(SizeEntry_t)]);
        size_ += 1;
    }
    template<typename U> void push_back(const U& value) {
        emplace_back<U>(value);
    }
    template<typename U> void push_back(U&& value) {
        emplace_back<U>(value);
    }
    void pop_back() {
        reinterpret_cast<T*>(&data_[sizeInBytes_ + sizeof(SizeEntry_t)])->~T();
        sizeInBytes_ -= *reinterpret_cast<SizeEntry_t*>(&data_[sizeInBytes_]);
    }
    T& back() {
        return *reinterpret_cast<T*>(&data_[sizeInBytes_ + sizeof(SizeEntry_t)]);
    }

    void reserve(int newCapacity) {
        data_.resize(newCapacity);
    }

    T& operator[](int ind) {
        return *reinterpret_cast<T*>(indexToOffset[ind]);
    }
    const T& operator[](int ind) const {
        return *reinterpret_cast<T*>(indexToOffset[ind]);
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

    Iterator begin() {
        return Iterator(data_.data() + sizeof(SizeEntry_t));
    }
    Iterator end() {
        return Iterator(data_.data() + sizeInBytes_ + *reinterpret_cast<SizeEntry_t*>(&data_[sizeInBytes_]) + sizeof(SizeEntry_t));
    }

private:
    template<typename T> static constexpr int EntrySize() {
        return sizeof(SizeEntry_t) + sizeof(T);
    }

    int size_;
    int sizeInBytes_;
    std::vector<NoInitByte> data_;
    std::vector<void*> indexToOffset;
};