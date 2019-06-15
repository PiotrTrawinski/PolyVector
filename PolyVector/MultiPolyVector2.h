#pragma once
#include <vector>
#include <tuple>
#include <unordered_map>
#include "classes.h"

template<typename T> class MultiPolyVector2 {
public:
    class Iterator {
    public:
        Iterator(std::vector<std::tuple<int, int, std::vector<std::byte>>>& vecs, int vecId = 0) :
            vecId(vecId),
            elemId(0),
            vecs(vecs)
        {}
        Iterator operator++() {
            elemId += 1;
            if (elemId >= std::get<1>(vecs[vecId])) {
                vecId += 1;
                elemId = 0;
            }
            return *this;
        }
        bool operator==(const Iterator& other) const {
            return vecId == other.vecId && elemId == other.elemId;
        }
        bool operator!=(const Iterator& other) const {
            return vecId != other.vecId || elemId != other.elemId;
        }
        T& operator*() const {
            return *reinterpret_cast<T*>(&std::get<2>(vecs[vecId])[elemId * std::get<0>(vecs[vecId])]);
        }
        T* operator->() const {
            return reinterpret_cast<T*>(&std::get<2>(vecs[vecId])[elemId * std::get<0>(vecs[vecId])]);
        }
    private:
        int vecId;
        int elemId;
        std::vector<std::tuple<int, int, std::vector<std::byte>>>& vecs;
    };

    MultiPolyVector2() {
        vecs.emplace_back(sizeof(T), 0, std::vector<std::byte>(50 * sizeof(T)));
        //sizeToVec.emplace(typeid(T).hash_code(), sizeToVec.size());
        sizeToVec.emplace_back(sizeof(T));
    }
    ~MultiPolyVector2() {
        for (auto vecIter = vBegin(); vecIter != vEnd(); ++vecIter) {
            for (auto& elem : vecIter) {
                elem.~T();
            }
        }
    }

    template<typename U, typename... Args> void emplace_back(Args&&... args) {
        //auto typeHash = typeid(U).hash_code();
        //if (auto vecIndex = sizeToVec.find(typeHash); vecIndex != sizeToVec.end()) {
        //    auto& [elemSize, elemCount, elements] = vecs[vecIndex->second];
        if (auto vecIndexIter = std::find(sizeToVec.begin(), sizeToVec.end(), sizeof(U));  vecIndexIter != sizeToVec.end()) {
            auto vecIndex = std::distance(sizeToVec.begin(), vecIndexIter);
            auto&[elemSize, elemCount, elements] = vecs[vecIndex];
            if (elemSize*elemCount >= elements.size()) {
                elements.resize(elements.size() * 2);
            }
            new (elements.data() + elemSize * elemCount) U(std::forward<Args>(args)...);
            elemCount += 1;
        }
        else {
            vecs.emplace_back(sizeof(U), 1, std::vector<std::byte>(reserveSize * sizeof(U)));
            //sizeToVec.emplace(typeid(U).hash_code(), sizeToVec.size());
            sizeToVec.emplace_back(sizeof(U));
            new (std::get<2>(vecs.back()).data()) U(std::forward<Args>(args)...);
        }
    }

    void reserve(int reserveSize) {
        this->reserveSize = reserveSize;
        for (auto& vec : vecs) {
            std::get<2>(vec).resize(std::get<0>(vec) * reserveSize);
        }
    }

    Iterator begin() {
        return Iterator(vecs);
    }
    Iterator end() {
        return Iterator(vecs, vecs.size());
    }


    class VectorIterator {
    public:
        class ElementIterator {
        public:
            ElementIterator(std::byte* ptr, int allignment) :
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
            std::byte* ptr;
            int allignment;
        };

        VectorIterator(std::vector<std::tuple<int, int, std::vector<std::byte>>>& vecs, int vecId = 0) :
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
        ElementIterator begin() {
            return ElementIterator(std::get<2>(vecs[vecId]).data(), std::get<0>(vecs[vecId]));
        }
        ElementIterator end() {
            return ElementIterator(
                std::get<2>(vecs[vecId]).data() + std::get<1>(vecs[vecId]) * std::get<0>(vecs[vecId]),
                std::get<0>(vecs[vecId])
            );
        }
    private:
        int vecId;
        std::vector<std::tuple<int, int, std::vector<std::byte>>>& vecs;
    };

    VectorIterator vBegin() {
        return VectorIterator(vecs, 0);
    }
    VectorIterator vEnd() {
        return VectorIterator(vecs, vecs.size());
    }

private:
    std::vector<std::tuple<int, int, std::vector<std::byte>>> vecs;
    std::vector<size_t> sizeToVec;
    //std::unordered_map<size_t, int> sizeToVec;
    int reserveSize = 50;
};