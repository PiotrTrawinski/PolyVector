#pragma once
#include <vector>
#include <tuple>
#include <unordered_map>
#include <typeindex>
#include "classes.h"
#include "NoInitByte.h"

template<typename T> class MultiPolyVector {
public:
    class Iterator {
    public:
        Iterator(std::vector<std::tuple<int, int, std::vector<NoInitByte>>>& vecs, int vecId, int currSize, NoInitByte* currPtr, NoInitByte* currEndPtr) :
            vecId(vecId),
            currSize(currSize),
            currPtr(currPtr),
            currEndPtr(currEndPtr),
            vecs(vecs)
        {}
        Iterator operator++() {
            currPtr += currSize;
            if (currPtr == currEndPtr) {
                vecId += 1;
                if (vecId < vecs.size()) {
                    currSize = std::get<0>(vecs[vecId]);
                    currPtr = std::get<2>(vecs[vecId]).data();
                    currEndPtr = std::get<2>(vecs[vecId]).data() + std::get<0>(vecs[vecId])*std::get<1>(vecs[vecId]);
                } else {
                    currSize = 0;
                    currPtr = nullptr;
                    currEndPtr = nullptr;
                }
            }
            return *this;
        }
        bool operator==(const Iterator& other) const {
            return currPtr == other.currPtr;
        }
        bool operator!=(const Iterator& other) const {
            return currPtr != other.currPtr;
        }
        T& operator*() const {
            return *reinterpret_cast<T*>(currPtr);
        }
        T* operator->() const {
            return reinterpret_cast<T*>(currPtr);
        }
    private:
        int vecId;
        int currSize;
        NoInitByte* currPtr;
        NoInitByte* currEndPtr;
        std::vector<std::tuple<int, int, std::vector<NoInitByte>>>& vecs;
    };

    MultiPolyVector(int reserveSize=50) : 
        reserveSize_(reserveSize)
    {}
    ~MultiPolyVector() {
        for (auto vecIter = vBegin(), vecEnd = vEnd(); vecIter != vecEnd; ++vecIter) {
            for (auto& elem : vecIter) {
                elem.~T();
            }
        }
    }
    template <typename U> void initialize() {
        static_assert(std::is_base_of<T, U>::value, "Not derived class");
        vecs.emplace_back(sizeof(U), 0, std::vector<NoInitByte>(reserveSize_ * sizeof(U)));
        getTypeIndex<U>();
    }
    template <typename U, typename V, typename... Rest> void initialize() {
        initialize<U>();
        initialize<V, Rest...>();
    }

    void setReserveSize(int reserveSize) {
        reserveSize_ = reserveSize;
    }
    int reserveSize() {
        return reserveSize_;
    }
    
    void reserve(int reserveSize) {
        for (auto& vec : vecs) {
            std::get<2>(vec).resize(std::get<0>(vec) * reserveSize);
        }
    }
    void reserve() {
        reserve(reserveSize_);
    }
    template <typename U> void reserve(int reserveSize) {
        static_assert(std::is_base_of<T, U>::value, "Not derived class");
        auto& vec = vecs[getTypeIndex<U>()];
        std::get<2>(vec).resize(std::get<0>(vec) * reserveSize);
    }
    template <typename U> void reserve() {
        reserve<U>(reserveSize_);
    }
    template <typename U, typename V, typename... Rest> void reserve(int reserveSize) {
        reserve<U>();
        reserve<V, Rest...>();
    }
    template <typename U, typename V, typename... Rest> void reserve() {
        reserve<U, V, Rest...>(reserveSize_);
    }

    template<typename U, typename... Args> void force_emplace_back(Args&&... args) {
        static_assert(std::is_base_of<T, U>::value, "Not derived class");
        auto&[elemSize, elemCount, elements] = vecs[getTypeIndex<U>()];
        if (elemSize*elemCount >= elements.size()) {
            elements.resize(elements.size() * 2);
        }
        new (elements.data() + elemSize * elemCount) U(std::forward<Args>(args)...);
        elemCount += 1;
    }
    template<typename U, typename... Args> void emplace_back(Args&&... args) {
        if (getTypeIndex<U>() < vecs.size()) {
            force_emplace_back<U>(args...);
        } else {
            vecs.emplace_back(sizeof(U), 1, std::vector<NoInitByte>(reserveSize_ * sizeof(U)));
            new (std::get<2>(vecs.back()).data()) U(std::forward<Args>(args)...);
        }
    }

    Iterator begin() {
        return Iterator(vecs, 0, std::get<0>(vecs[0]), std::get<2>(vecs[0]).data(), std::get<2>(vecs[0]).data() + std::get<0>(vecs[0])*std::get<1>(vecs[0]));
    }
    Iterator end() {
        return Iterator(vecs, 0, vecs.size(), nullptr, nullptr);
    }

    class VectorIterator {
    public:
        class ElementIterator {
        public:
            ElementIterator(NoInitByte* ptr, int allignment) :
                ptr(ptr),
                allignment(allignment)
            {}
            ElementIterator operator++() {
                ptr += allignment;
                return *this;
            }
            bool operator==(const ElementIterator& other) const {
                return ptr == other.ptr;
            }
            bool operator!=(const ElementIterator& other) const {
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

        class ElementRemoveIterator {
        public:
            ElementRemoveIterator(NoInitByte* ptr, NoInitByte* endPtr, int allignment) :
                ptr(ptr),
                endPtr(endPtr),
                allignment(allignment)
            {}
            ElementRemoveIterator operator++() {
                ptr += allignment;
                return *this;
            }
            bool notAtEnd() {
                return ptr != endPtr;
            }
            T& operator*() const {
                return *reinterpret_cast<T*>(ptr);
            }
            T* operator->() const {
                return reinterpret_cast<T*>(ptr);
            }
            void remove() {
                reinterpret_cast<T*>(ptr)->~T();
                endPtr -= allignment;
                memcpy(ptr, endPtr, allignment);
                ptr -= allignment;
                removedElementsCount += 1;
            }
            int getRemovedElementsCount() {
                return removedElementsCount;
            }
        private:
            NoInitByte* ptr;
            NoInitByte* endPtr;
            int allignment;
            int removedElementsCount = 0;
        };

        VectorIterator(std::vector<std::tuple<int, int, std::vector<NoInitByte>>>& vecs, int vecId = 0) :
            vecId(vecId),
            vecs(vecs)
        {}
        VectorIterator operator++() {
            vecId += 1;
            return *this;
        }
        bool operator==(const VectorIterator& other) const {
            return vecId == other.vecId;
        }
        bool operator!=(const VectorIterator& other) const {
            return vecId != other.vecId;
        }
        std::tuple<int, int, std::vector<NoInitByte>>& operator*() const {
            return vecs[vecId];
        }
        std::tuple<int, int, std::vector<NoInitByte>>* operator->() const {
            return &vecs[vecId];
        }
        ElementIterator begin() {
            return ElementIterator(std::get<2>(vecs[vecId]).data(), std::get<0>(vecs[vecId]));
        }
        ElementIterator end() {
            return ElementIterator(
                std::get<2>(vecs[vecId]).data() + std::get<1>(vecs[vecId]) * std::get<0>(vecs[vecId]), 
                std::get<0>(vecs[vecId])
            );
        }
        ElementRemoveIterator removeIterator() {
            return ElementRemoveIterator(
                std::get<2>(vecs[vecId]).data(), 
                std::get<2>(vecs[vecId]).data() + std::get<1>(vecs[vecId]) * std::get<0>(vecs[vecId]),
                std::get<0>(vecs[vecId])
            );
        }
    private:
        int vecId;
        std::vector<std::tuple<int, int, std::vector<NoInitByte>>>& vecs;    
    };

    VectorIterator vBegin() {
        return VectorIterator(vecs, 0);
    }
    VectorIterator vEnd() {
        return VectorIterator(vecs, vecs.size());
    }

    template<typename Function> void foreach(Function function) {
        for (auto vecIter = vBegin(), vecEnd = vEnd(); vecIter != vecEnd; ++vecIter) {
            for (auto& elem : vecIter) {
                function(elem);
            }
        }
    }
    template<typename Function> void foreachRemove(Function function) {
        for (auto vecIter = vBegin(), vecEnd = vEnd(); vecIter != vecEnd; ++vecIter) {
            auto elemIter = vecIter.removeIterator();
            for (; elemIter.notAtEnd(); ++elemIter) {
                function(elemIter);
            }
            std::get<1>(*vecIter) -= elemIter.getRemovedElementsCount();
        }
    }
    template<typename Function> void foreachRemoveIf(Function function) {
        for (auto vecIter = vBegin(), vecEnd = vEnd(); vecIter != vecEnd; ++vecIter) {
            auto elemIter = vecIter.removeIterator();
            for (; elemIter.notAtEnd(); ++elemIter) {
                if (function(*elemIter)) {
                    elemIter.remove();
                }
            }
            std::get<1>(*vecIter) -= elemIter.getRemovedElementsCount();
        }
    }

private:
    class TypeIndex {
        size_t counter = 0;
    public:
        template<typename T> size_t value() {
            static size_t id = counter++;
            return id;
        }
    };
    template <typename T> size_t getTypeIndex() noexcept {
        return typeIndex.value<T>();
    }

    std::vector<std::tuple<int, int, std::vector<NoInitByte>>> vecs;
    TypeIndex typeIndex;
    int reserveSize_ = 50;
};

#define foreachMulti(ELEMENT, VECTOR) \
for (auto vecIter = VECTOR.vBegin(), vecEnd = VECTOR.vEnd(); vecIter != vecEnd; ++vecIter) \
    for (auto& ELEMENT : vecIter)