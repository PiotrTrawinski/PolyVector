#pragma once
#include <tuple>
#include <chrono>

template<typename ReturnType, typename Function, typename... Args>
std::pair<ReturnType, double> benchmark(Function function, Args... args) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    ReturnType result = function(args...);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = duration_cast<milliseconds>(end - start).count();
    return std::pair(result, time / 1000.0);
}
template<typename Function, typename... Args> double benchmark(Function function, Args... args) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    function(args...);
    auto end = high_resolution_clock::now();
    auto time = duration_cast<milliseconds>(end - start).count();
    return time / 1000.0;
}
template<typename Function, typename... Args> double benchmark(int repetitions, Function function, Args... args) {
    double time = 0;
    for (int i = 0; i < repetitions; ++i) {
        time += benchmark(function, args...);
    }
    return time / repetitions;
}

struct MyTime {
    MyTime() : time(std::chrono::high_resolution_clock::now()) {}
    double operator-(const MyTime& other) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(time - other.time).count() / 1000.0;
    }
private:
    std::chrono::high_resolution_clock::time_point time;
};
MyTime getTime() {
    return MyTime();
}
