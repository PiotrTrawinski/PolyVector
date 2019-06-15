#include <iostream>
#include <tuple>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <string>
#include "classes.h"
#include "AllignedPolyVector.h"
#include "PackedPolyVector.h"
#include "IndexPackedPolyVector.h"
#include "MultiPolyVector.h"
#include "MultiPolyVector2.h"
#include "HardCodedMultiPolyVector.h"
#include "benchmark.h"

template<typename T, typename PolyVector, typename... Args>
void emplace_back(PolyVector& vec, Args... args) {
    vec.emplace_back<T>(args...);
}
template<typename T, typename... Args>
void emplace_back(std::vector<std::unique_ptr<Base>>& vec, Args... args) {
    vec.emplace_back(std::make_unique<T>(args...));
}
/*template<typename T, typename... Args>
void emplace_back(MultiPolyVector<Base>& vec, Args... args) {
    vec.force_emplace_back<T>(args...);
}*/
template<typename Vector> void fill(Vector& vec, const int N) {
    /*std::mt19937 gen(1234);
    std::uniform_int_distribution<int> dist(0, 100);
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
        case 0: emplace_back<Base>(vec, dist(gen)); break;
        case 1: emplace_back<Derived>(vec, dist(gen), dist(gen)); break;
        case 2: emplace_back<Derived2>(vec, dist(gen), dist(gen)); break;
        case 3: emplace_back<Derived3>(vec, dist(gen), dist(gen)); break;
        }
    }*/
    int x = 0;
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
        case 0: emplace_back<Base>(vec, x); break;
        case 1: emplace_back<Derived>(vec, x, x+1); break;
        case 2: emplace_back<Derived2>(vec, x, x+1); break;
        case 3: emplace_back<Derived3>(vec, x, x+1); break;
        }
        x += 2;
    }
}
template<typename PolyVector> int sum(PolyVector& vec) {
    int sum = 0;
    //for (int i = 0; i < 20; ++i) {
        for (auto& b : vec) {
            sum += b.getData();
        }
    //}
    return sum;
}
int sum(std::vector<std::unique_ptr<Base>>& vec) {
    int sum = 0;
    //for (int i = 0; i < 10; ++i) {
        for (auto& b : vec) {
            sum += b->getData();
        }
        /*int number = 0;
        int ind = 0;
        while (ind < vec.size()) {
            auto& elem = vec[ind];
            sum += elem->getData();
            number += 1;
            if (elem->getData() % (i+3)) {
                std::swap(elem, vec.back());;
                vec.pop_back();
            } else {
                ind += 1;
                elem->setData(elem->getData() + 1);
            }
        }
        std::cout << number << '\n';*/
    //}
    return sum;
}
int sum(MultiPolyVector<Base>& polyVec) {
    int sum = 0;
    //for (int i = 0; i < 10; ++i) {
        polyVec.foreach([&sum](auto& elem) {
            sum += elem.getData();
        });
        //int number = 0;
        /*polyVec.foreachRemove([&sum, i, &number](auto& elem) {
            sum += elem->getData();
            number += 1;
            if (elem->getData() % (i+3)) {
                elem.remove();
            } else {
                elem->setData(elem->getData()+1);
            }
        });*/
        /*polyVec.foreachRemoveIf([&sum, i, &number](auto& elem) {
            sum += elem.getData();
            number += 1;
            if (elem.getData() % (i + 3)) {
                return true;
            } else {
                elem.setData(elem.getData() + 1);
            }
            return false;
        });
        std::cout << number << '\n';*/
    //}
    return sum;
}
int sum(HardCodedMultiPolyVector& polyVec) {
    int sum = 0;
    //for (int i = 0; i < 20; ++i) {
        for (auto& vec : polyVec.baseVec) {
            sum += vec.getData();
        }
        for (auto& vec : polyVec.derivedVec) {
            sum += vec.getData();
        }
        for (auto& vec : polyVec.derived2Vec) {
            sum += vec.getData();
        }
        for (auto& vec : polyVec.derived3Vec) {
            sum += vec.getData();
        }
    //}
    return sum;
}


template<typename T> void fillSumBenchmark(T& vec, const int N) {
    int sumValue = 0;

    auto fillTime = benchmark([&]() { fill(vec, N); });
    auto sumTime = benchmark(1, [&]() { sumValue = sum(vec); });

    std::cout << "sum = " << sumValue << '\n';
    std::cout << "fillTime    = " << fillTime << '\n';
    std::cout << "sumTime     = " << sumTime << '\n';
}

template<typename Vector, typename... Args>
void speedTest1(const std::string& name, const int N, Args... args) {
    std::cout << name << ": \n";

    MyTime deallocationStart;
    {
        Vector vec(args...);
        fillSumBenchmark(vec, N);
        deallocationStart = getTime();
    }
    auto deallocationEnd = getTime();

    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
    std::cout << '\n';
}
void speedTest1Multi(const std::string& name, const int N) {
    std::cout << name << ": \n";

    MyTime deallocationStart;
    {
        MultiPolyVector<Base> vec;
        vec.initialize<Base, Derived, Derived2, Derived3>();
        fillSumBenchmark(vec, N);
        deallocationStart = getTime();
    }
    auto deallocationEnd = getTime();

    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
    std::cout << '\n';
}
template<typename Vector, typename... Args>
void speedTest2(const std::string& name, const int N, const int reserveArg, Args... args) {
    std::cout << name << ": \n";

    MyTime deallocationStart;
    {
        Vector vec(args...);
        vec.reserve(reserveArg);
        fillSumBenchmark(vec, N);
        deallocationStart = getTime();
    }
    auto deallocationEnd = getTime();

    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
    std::cout << '\n';
}
void speedTest2Multi(const std::string& name, const int N, const int reserveArg) {
    std::cout << name << ": \n";

    MyTime deallocationStart;
    {
        MultiPolyVector<Base> vec;
        vec.initialize<Base, Derived, Derived2, Derived3>();
        vec.reserve(reserveArg);
        
        int sumValue = 0;

        auto fillTime = benchmark([&]() { 
            int x = 0;
            for (int i = 0; i < N; ++i) {
                switch (i % 4) {
                case 0: vec.force_emplace_back<Base>(x); break;
                case 1: vec.force_emplace_back<Derived>(x, x + 1); break;
                case 2: vec.force_emplace_back<Derived2>(x, x + 1); break;
                case 3: vec.force_emplace_back<Derived3>(x, x + 1); break;
                }
                x += 2;
            }
        });
        auto sumTime = benchmark(1, [&]() { sumValue = sum(vec); });

        std::cout << "sum = " << sumValue << '\n';
        std::cout << "fillTime    = " << fillTime << '\n';
        std::cout << "sumTime     = " << sumTime << '\n';

        deallocationStart = getTime();
    }
    auto deallocationEnd = getTime();

    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
    std::cout << '\n';
}
void speedTest() {
    constexpr int N = 3e7;

    speedTest1<std::vector<std::unique_ptr<Base>>>("std::vector<std::unique_ptr<>>", N);
    speedTest1<AllignedPolyVector<Base>>("AllignedPolyVector", N, maxTypeSize<Base, Derived, Derived2, Derived3>());
    speedTest1<PackedPolyVector<Base>>("PackedPolyVector", N);
    speedTest1<IndexPackedPolyVector<Base>>("IndexPackedPolyVector", N);
    speedTest1<MultiPolyVector<Base>>("MultiPolyVector", N);

    //speedTest2<std::vector<std::unique_ptr<Base>>>("std::vector<std::unique_ptr<>>", N, N);
    //speedTest2<AllignedPolyVector<Base>>("AllignedPolyVector", N, N, maxTypeSize<Base, Derived, Derived2, Derived3>());
    //speedTest2<PackedPolyVector<Base>>("PackedPolyVector", N, N*sizeof(Derived));
    //speedTest2<IndexPackedPolyVector<Base>>("IndexPackedPolyVector", N, N * sizeof(Derived));
    //speedTest2<MultiPolyVector<Base>>("MultiPolyVector", N, N / 4);
    //speedTest2Multi("MultiPolyVector", N, N / 4);
}

int main() {
    speedTest();

    std::cin.get();
    return 0;
}





