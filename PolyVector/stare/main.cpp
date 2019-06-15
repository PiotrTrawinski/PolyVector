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
#include "benchmark.h"

template<typename PolyVector> void fill(PolyVector& vec, const int N) {
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
void fill(std::vector<std::unique_ptr<Base>>& vec, const int N) {
    std::mt19937 gen(1234);
    std::uniform_int_distribution<int> dist(0, 100);
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
        case 0: vec.emplace_back(std::make_unique<Base>(dist(gen))); break;
        case 1: vec.emplace_back(std::make_unique<Derived>(dist(gen), dist(gen))); break;
        case 2: vec.emplace_back(std::make_unique<Derived2>(dist(gen), dist(gen))); break;
        case 3: vec.emplace_back(std::make_unique<Derived3>(dist(gen), dist(gen))); break;
        }
    }
}
template<typename PolyVector> int sum(PolyVector& vec) {
    int sum = 0;
    for (auto& b : vec) {
        sum += b.getData();
    }
    return sum;
}
int sum(std::vector<std::unique_ptr<Base>>& vec) {
    int sum = 0;
    for (auto& b : vec) {
        sum += b->getData();
    }
    return sum;
}


template<typename T> void fillSumBenchmark(T& vec, const int N) {
    int sumValue = 0;

    auto fillTime = benchmark([&]() { fill(vec, N); });
    auto sumTime = benchmark([&]() { sumValue = sum(vec); });

    std::cout << "sum = " << sumValue << '\n';
    std::cout << "fillTime    = " << fillTime << '\n';
    std::cout << "sumTime     = " << sumTime << '\n';
}

template<typename PolyVector, typename... Args> 
void speedTest3PolyVector(const std::string& name, const int N, Args... args) {
    std::cout << name << ": \n";

    MyTime deallocationStart;
    {
        PolyVector polyVec(args...);
        fillSumBenchmark(polyVec, N);
        deallocationStart = getTime();
    }
    auto deallocationEnd = getTime();

    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
    std::cout << '\n';
}
void speedTest3() {
    constexpr int N = 3e7;
    speedTest3PolyVector<std::vector<std::unique_ptr<Base>>>("std::vector<std::unique_ptr<>>", N);
    speedTest3PolyVector<AllignedPolyVector<Base>>("AllignedPolyVector", N, maxTypeSize<Base, Derived, Derived2, Derived3>());
    speedTest3PolyVector<PackedPolyVector<Base>>("PackedPolyVector", N);
    speedTest3PolyVector<IndexPackedPolyVector<Base>>("IndexPackedPolyVector", N);
}




//template<typename T> void reserveFillSumBenchmark(T& vec, const int N) {
//    int sumValue = 0;
//    
//    //auto reserveTime = benchmark([&]() { vec.reserve(N); });
//    auto fillTime = benchmark([&]() { fill(vec, N); });
//    auto sumTime = benchmark([&]() { sumValue = sum(vec); });
//
//    std::cout << "sum = " << sumValue << '\n';
//    //std::cout << "reserveTime = " << reserveTime << '\n';
//    std::cout << "fillTime    = " << fillTime << '\n';
//    std::cout << "sumTime     = " << sumTime << '\n';
//}
//
//void speedTest() {
//    //constexpr int N = 10;
//    constexpr int N = 3e7;
//    MyTime deallocationStart;
//    MyTime deallocationEnd;
//
//    std::cout << "std::vector: \n";
//    {
//        std::vector<Base*> vec;
//
//        reserveFillSumBenchmark(vec, N);
//
//        auto deletePtrTime = benchmark([&]() {
//            for (auto* b : vec) {
//                delete b;
//            }
//        });
//        std::cout << "deletePtr = " << deletePtrTime << '\n';
//        deallocationStart = getTime();
//    }
//    deallocationEnd = getTime();
//    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
//    std::cout << '\n';
//
//    
//    std::cout << "std::vector<std::unique_ptr<>>: \n";
//    {
//        std::vector<std::unique_ptr<Base>> vec;
//
//        reserveFillSumBenchmark(vec, N);
//        deallocationStart = getTime();
//    }
//    deallocationEnd = getTime();
//    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
//    std::cout << '\n';
//
//
//    // PolyVec
//    std::cout << "PolyVec: \n";
//    {
//        PolyVec<Base> polyVec(maxTypeSize<Base, Derived, Derived2, Derived3>());
//
//        reserveFillSumBenchmark(polyVec, N);
//        deallocationStart = getTime();
//    }
//    deallocationEnd = getTime();
//    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
//    std::cout << '\n';
//
//
//    // PackedPolyVec
//    std::cout << "PackedPolyVec: \n";
//    {
//        PackedPolyVec<Base> packedPolyVec;
//
//        reserveFillSumBenchmark(packedPolyVec, N);
//        deallocationStart = getTime();
//    }
//    deallocationEnd = getTime();
//    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
//    std::cout << '\n';
//
//    // IndexPackedPolyVec
//    std::cout << "IndexPackedPolyVec: \n";
//    {
//        IndexPackedPolyVec<Base> indexPackedPolyVec;
//
//        reserveFillSumBenchmark(indexPackedPolyVec, N);
//        deallocationStart = getTime();
//    }
//    deallocationEnd = getTime();
//    std::cout << "deallocation = " << deallocationEnd - deallocationStart << '\n';
//    std::cout << '\n';
//
//    // PolyVec3
//    /*PolyVec3<Base, Derived, Derived2, Derived3> polyVec3;
//    polyVec3.reserve(N);
//    auto poly3FillTime = benchmark([&polyVec3, N]() {
//        fill(polyVec3, N);
//    });
//    auto poly3SumTime = benchmark(1, [&polyVec3, N]() {
//        std::cout << sum(polyVec3) << '\n';
//    });
//    std::cout << "poly3FillTime  = " << poly3FillTime << '\n';
//    std::cout << "poly3SumTime   = " << poly3SumTime << '\n';
//    std::cout << '\n';*/
//}
//void speedTest2() {
//    constexpr int N = 1e7;
//
//    std::cout << "std::vector<*>: \n";
//    {
//        std::mt19937 gen(1234);
//        std::uniform_int_distribution<int> dist(0, 100);
//
//        std::vector<Base*> vec1;
//        std::vector<Base*> vec2;
//        std::vector<Base*> vec3;
//
//        vec1.reserve(N);
//        vec2.reserve(N);
//        vec3.reserve(N);
//
//        for (int i = 0; i < N * 3; ++i) {
//            switch (i % 3) {
//            case 0: {
//                switch (i % 4) {
//                case 0: vec1.emplace_back(new Base(dist(gen))); break;
//                case 1: vec1.emplace_back(new Derived(dist(gen), dist(gen))); break;
//                case 2: vec1.emplace_back(new Derived2(dist(gen), dist(gen))); break;
//                case 3: vec1.emplace_back(new Derived3(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            case 1: {
//                switch (i % 4) {
//                case 0: vec2.emplace_back(new Base(dist(gen))); break;
//                case 1: vec2.emplace_back(new Derived(dist(gen), dist(gen))); break;
//                case 2: vec2.emplace_back(new Derived2(dist(gen), dist(gen))); break;
//                case 3: vec2.emplace_back(new Derived3(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            case 2: {
//                switch (i % 4) {
//                case 0: vec3.emplace_back(new Base(dist(gen))); break;
//                case 1: vec3.emplace_back(new Derived(dist(gen), dist(gen))); break;
//                case 2: vec3.emplace_back(new Derived2(dist(gen), dist(gen))); break;
//                case 3: vec3.emplace_back(new Derived3(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            }
//        }
//
//        auto sumTime1 = benchmark(10, [&]() { sum(vec1); });
//        auto sumTime2 = benchmark(10, [&]() { sum(vec2); });
//        auto sumTime3 = benchmark(10, [&]() { sum(vec3); });
//
//        std::cout << "sumTime1 = " << sumTime1 << '\n';
//        std::cout << "sumTime2 = " << sumTime2 << '\n';
//        std::cout << "sumTime3 = " << sumTime3 << '\n';
//    }
//    std::cout << '\n';
//
//
//    std::cout << "std::vector<std::unique_ptr<>>: \n";
//    {
//        std::mt19937 gen(1234);
//        std::uniform_int_distribution<int> dist(0, 100);
//
//        std::vector<std::unique_ptr<Base>> vec1;
//        std::vector<std::unique_ptr<Base>> vec2;
//        std::vector<std::unique_ptr<Base>> vec3;
//
//        vec1.reserve(N);
//        vec2.reserve(N);
//        vec3.reserve(N);
//
//        for (int i = 0; i < N * 3; ++i) {
//            switch (i % 3) {
//            case 0: {
//                switch (i % 4) {
//                case 0: vec1.emplace_back(std::make_unique<Base>(dist(gen))); break;
//                case 1: vec1.emplace_back(std::make_unique<Derived>(dist(gen), dist(gen))); break;
//                case 2: vec1.emplace_back(std::make_unique<Derived2>(dist(gen), dist(gen))); break;
//                case 3: vec1.emplace_back(std::make_unique<Derived3>(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            case 1: {
//                switch (i % 4) {
//                case 0: vec2.emplace_back(std::make_unique<Base>(dist(gen))); break;
//                case 1: vec2.emplace_back(std::make_unique<Derived>(dist(gen), dist(gen))); break;
//                case 2: vec2.emplace_back(std::make_unique<Derived2>(dist(gen), dist(gen))); break;
//                case 3: vec2.emplace_back(std::make_unique<Derived3>(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            case 2: {
//                switch (i % 4) {
//                case 0: vec3.emplace_back(std::make_unique<Base>(dist(gen))); break;
//                case 1: vec3.emplace_back(std::make_unique<Derived>(dist(gen), dist(gen))); break;
//                case 2: vec3.emplace_back(std::make_unique<Derived2>(dist(gen), dist(gen))); break;
//                case 3: vec3.emplace_back(std::make_unique<Derived3>(dist(gen), dist(gen))); break;
//                }
//                break;
//            }
//            }
//        }
//
//        auto sumTime1 = benchmark(10, [&]() { sum(vec1); });
//        auto sumTime2 = benchmark(10, [&]() { sum(vec2); });
//        auto sumTime3 = benchmark(10, [&]() { sum(vec3); });
//
//        std::cout << "sumTime1 = " << sumTime1 << '\n';
//        std::cout << "sumTime2 = " << sumTime2 << '\n';
//        std::cout << "sumTime3 = " << sumTime3 << '\n';
//    }
//    std::cout << '\n';
//
//
//    std::cout << "PolyVec: \n";
//    {
//        
//        std::mt19937 gen(1234);
//        std::uniform_int_distribution<int> dist(0, 100);
//
//        PolyVec<Base> vec1(maxTypeSize<Base, Derived, Derived2, Derived3>());
//        PolyVec<Base> vec2(maxTypeSize<Base, Derived, Derived2, Derived3>());
//        PolyVec<Base> vec3(maxTypeSize<Base, Derived, Derived2, Derived3>());
//
//        vec1.reserve(N);
//        vec2.reserve(N);
//        vec3.reserve(N);
//
//        for (int i = 0; i < N * 3; ++i) {
//            switch (i % 3) {
//            case 0: {
//                switch (i % 4) {
//                case 0: vec1.emplace_back<Base>(dist(gen)); break;
//                case 1: vec1.emplace_back<Derived>(dist(gen), dist(gen)); break;
//                case 2: vec1.emplace_back<Derived2>(dist(gen), dist(gen)); break;
//                case 3: vec1.emplace_back<Derived3>(dist(gen), dist(gen)); break;
//                }
//                break;
//            }
//            case 1: {
//                switch (i % 4) {
//                case 0: vec2.emplace_back<Base>(dist(gen)); break;
//                case 1: vec2.emplace_back<Derived>(dist(gen), dist(gen)); break;
//                case 2: vec2.emplace_back<Derived2>(dist(gen), dist(gen)); break;
//                case 3: vec2.emplace_back<Derived3>(dist(gen), dist(gen)); break;
//                }
//                break;
//            }
//            case 2: {
//                switch (i % 4) {
//                case 0: vec3.emplace_back<Base>(dist(gen)); break;
//                case 1: vec3.emplace_back<Derived>(dist(gen), dist(gen)); break;
//                case 2: vec3.emplace_back<Derived2>(dist(gen), dist(gen)); break;
//                case 3: vec3.emplace_back<Derived3>(dist(gen), dist(gen)); break;
//                }
//                break;
//            }
//            }
//        }
//
//        auto sumTime1 = benchmark(10, [&]() { sum(vec1); });
//        auto sumTime2 = benchmark(10, [&]() { sum(vec2); });
//        auto sumTime3 = benchmark(10, [&]() { sum(vec3); });
//
//        std::cout << "sumTime1 = " << sumTime1 << '\n';
//        std::cout << "sumTime2 = " << sumTime2 << '\n';
//        std::cout << "sumTime3 = " << sumTime3 << '\n';
//    }
//    std::cout << '\n';
//
//    // PolyVec3
//    /*PolyVec3<Base, Derived, Derived2, Derived3> polyVec3;
//    polyVec3.reserve(N);
//    auto poly3FillTime = benchmark([&polyVec3, N]() {
//        fill(polyVec3, N);
//    });
//    auto poly3SumTime = benchmark(1, [&polyVec3, N]() {
//        std::cout << sum(polyVec3) << '\n';
//    });
//    std::cout << "poly3FillTime  = " << poly3FillTime << '\n';
//    std::cout << "poly3SumTime   = " << poly3SumTime << '\n';
//    std::cout << '\n';*/
//}
//
//void destructorTest() {
//    constexpr int N = 10;
//
//    // vector<*>
//    std::cout << "std::vector:\n";
//    {
//        std::vector<Base*> vec;
//        vec.reserve(N);
//        fill(vec, N);
//        std::cout << '\n';
//        for (auto* b : vec) {
//            delete b;
//        }
//    }
//
//    // vector<unique_ptr<>>
//    std::cout << "std::vector:\n";
//    {
//        std::vector<std::unique_ptr<Base>> vec;
//        vec.reserve(N);
//        fill(vec, N);
//        std::cout << '\n';
//    }
//    
//    // PolyVec
//    std::cout << "PolyVec:\n";
//    {
//        PolyVec<Base> polyVec(maxTypeSize<Base, Derived, Derived2, Derived3>());
//        polyVec.reserve(N);
//        fill(polyVec, N);
//        std::cout << '\n';
//    }
//
//    // PackedPolyVec
//    std::cout << "PackedPolyVec:\n";
//    {
//        PackedPolyVec<Base> packedPolyVec;
//        packedPolyVec.reserve(N * sizeof(Derived));
//        fill(packedPolyVec, N);
//        std::cout << '\n';
//    }
//}

int main() {
    //destructorTest();
    //speedTest();
    speedTest3();

    std::cin.get();

    return 0;
}





