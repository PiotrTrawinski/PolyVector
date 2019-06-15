#pragma once
#include <vector>
#include <random>
#include "classes.h"

void fill(std::vector<Base*>& vec, const int N) {
    std::mt19937 gen(1234);
    std::uniform_int_distribution<int> dist(0, 100);
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
        case 0: vec.emplace_back(new Base(dist(gen))); break;
        case 1: vec.emplace_back(new Derived(dist(gen), dist(gen))); break;
        case 2: vec.emplace_back(new Derived2(dist(gen), dist(gen))); break;
        case 3: vec.emplace_back(new Derived3(dist(gen), dist(gen))); break;
        }
        /*if (i % 2) {
            vec.emplace_back(new Base(dist(gen)));
        } else {
            vec.emplace_back(new Derived(dist(gen), dist(gen)));
        }*/
    }
}
int sum(std::vector<Base*>& vec) {
    int sum = 0;
    for (auto b : vec) {
        sum += b->getData();
    }
    return sum;
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
int sum(std::vector<std::unique_ptr<Base>>& vec) {
    int sum = 0;
    for (auto& b : vec) {
        sum += b->getData();
    }
    return sum;
}