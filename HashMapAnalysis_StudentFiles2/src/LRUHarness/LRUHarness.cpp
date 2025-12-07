#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <filesystem>

#include "implementations/HashNew/HashTableDictionary.hpp"
#include "Operations.hpp"

namespace fs = std::filesystem;

static std::size_t tableSizeForN(std::size_t N) {
    return static_cast<std::size_t>(N * 1.3);
}

bool load_trace_strict_header(const std::string &filename, std::size_t &declaredN, std::vector<Operation> &ops) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Cannot open: " << filename << "\n";
        return false;
    }

    std::string profile;
    unsigned seed;

    if (!(in >> profile >> declaredN >> seed))
        return false;

    ops.clear();
    std::string op, a, b;

    while (in >> op) {
        if (op == "I") {
            in >> a >> b;
            ops.emplace_back(OpCode::Insert, a + " " + b);
        }
        else if (op == "E") {
            in >> a >> b;
            ops.emplace_back(OpCode::Erase, a + " " + b);
        }
    }
    return true;
}

void replay(HashTableDictionary &tab, const std::vector<Operation> &ops) {
    for (auto &op : ops) {
        if (op.tag == OpCode::Insert)
            tab.insert(op.key);
        else
            tab.remove(op.key);
    }
}

double timedRun(std::size_t tableSize, HashTableDictionary::PROBE_TYPE pType, const std::vector<Operation> &ops) {
    {
        // to "warm" up the system before starting the actual test runs
        HashTableDictionary warm(tableSize, pType, true);
        replay(warm, ops);
    }

    std::vector<double> times;
    times.reserve(7);


    for (int i = 0; i < 7; i++) {
        HashTableDictionary t(tableSize, pType, true);
        auto t0 = std::chrono::steady_clock::now();
        replay(t, ops);
        auto t1 = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
    }

    std::sort(times.begin(), times.end());
    return times[3];
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./LRUHarness <traceFile>\n";
        return 1;
    }

    std::string tracePath = argv[1];
    std::size_t N = 0;
    std::vector<Operation> ops;

    if (!load_trace_strict_header(tracePath, N, ops)) {
        std::cerr << "Error: cannot load trace.\n";
        return 1;
    }

    const std::size_t M = tableSizeForN(N);

    std::size_t inserts = 0, erases = 0;
    for (auto &op : ops)
        (op.tag == OpCode::Insert ? inserts++ : erases++);

    fs::create_directories("csvs");
    fs::path csvfile = "csvs/lru_results.csv";

    if (!fs::exists(csvfile)) {
        std::ofstream h(csvfile);
        h << "impl,profile,trace_path,N,seed,elapsed_ms,ops_total,"
          << "inserts,erases,table_size,"
          << HashTableDictionary::csvStatsHeader()
          << ",probe_type,compaction_state\n";
    }

    {
        double ms = timedRun(M, HashTableDictionary::DOUBLE, ops);
        HashTableDictionary tab(M, HashTableDictionary::DOUBLE, true);
        replay(tab, ops);

        tab.writeCsvLine(csvfile.string(),"double_hash","lru_profile",tracePath,N, 23, ms,ops.size(),inserts, erases);

        // used to identify final outcome easier when recording results
        std::cout << "\n==== DOUBLE PROBING ====\n";
        tab.printStats();
        tab.printMask();
        tab.printBeforeAndAfterCompactionMaps();
    }

    {
        double ms = timedRun(M, HashTableDictionary::SINGLE, ops);
        HashTableDictionary tab(M, HashTableDictionary::SINGLE, true);
        replay(tab, ops);

        tab.writeCsvLine(csvfile.string(),"single_hash","lru_profile",tracePath,N, 23, ms,ops.size(),inserts, erases);

        // used to identify final outcome easier when recording results
        std::cout << "\n==== SINGLE PROBING ====\n";
        tab.printStats();
        tab.printMask();
        tab.printBeforeAndAfterCompactionMaps();
    }

    return 0;
}
