#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"
#include "system/MappedChunk.h"

using namespace std;

uint64_t fib(uint64_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib(n-1) + fib(n-2);
}

vector<uint64_t> times;

int main(int argc, char *argv[]) {

    uint64_t n = 0;
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help message")
        (",n", po::value<uint64_t>()->required(), "Calc fib(<arg>)")
        (",r", po::value<uint64_t>()->default_value(10), "Repeat benchmark <arg> times");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        cerr << desc << endl;
        return 0;
    }

    po::notify(vm);
    n = vm["-n"].as<uint64_t>();
    uint64_t runs = vm["-r"].as<uint64_t>();

    CycleTimer t;
    uint64_t result;
    for (unsigned i = 0; i < runs; i++) {
        //t.start();
        auto start = std::chrono::high_resolution_clock::now();
        result = fib(n);
        auto end = std::chrono::high_resolution_clock::now();
        //uint64_t time = t.stop();
        long time = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        times.push_back(time);
        asm volatile("" :: "g"(&result), "g"(&n) : "memory");
    }
    cout << "fib(" << n << ") = " << result << endl;

    for (auto it = times.begin(); it != times.end(); ++it) {
        std::cout << *it << ", ";
    }

    cout << endl;
    return 0;
}