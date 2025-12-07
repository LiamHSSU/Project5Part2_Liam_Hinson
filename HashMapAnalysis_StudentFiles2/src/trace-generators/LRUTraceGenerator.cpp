#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <random>

std::string WORD_FILE;

std::vector<std::pair<std::string,std::string>> buildWordBag(std::size_t N, unsigned seed) {
    std::ifstream in(WORD_FILE);
    if (!in.is_open()) {
        std::cerr << "ERROR: Cannot open word file: " << WORD_FILE << "\n";
        exit(1);
    }
    std::cout << "[TRACE GENERATOR] Successfully opened: " << WORD_FILE << "\n";


    std::vector<std::string> words;
    words.reserve(4 * N + 16);

    std::string w;
    for (std::size_t i = 0; i < 4 * N; i++) {
        if (!(in >> w)) {
            std::cerr << "Word file does not contain enough tokens.\n";
            exit(1);
        }
        words.push_back(w);
    }

    std::vector<std::pair<std::string,std::string>> bag;
    bag.reserve(12 * N);


    for (std::size_t i = 0; i < N; i++) {
        bag.emplace_back(words[i], words[i + 1]);
    }


    for (std::size_t i = N; i < 2 * N; i++) {
        for (int r = 0; r < 5; r++)
            bag.emplace_back(words[i], words[i + 1]);
    }


    for (std::size_t i = 2 * N; i < 4 * N; i++) {
        for (int r = 0; r < 3; r++)
            bag.emplace_back(words[i], words[i + 1]);
    }

    std::mt19937 rng(seed);
    std::shuffle(bag.begin(), bag.end(), rng);

    return bag;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./LRUTraceGenerator <N> <wordfile.txt>\n";
        return 1;
    }

    std::size_t N = std::stoul(argv[1]);
    WORD_FILE = argv[2];
    unsigned seed = 23;


    std::string outName =
            "../lruTraces/lru_profile_N_" + std::to_string(N) + "_S_" +
            std::to_string(seed) + ".trace";


    std::ofstream out(outName);
    if (!out.is_open()) {
        std::cerr << "Cannot write trace file.\n";
        return 1;
    }

    auto bag = buildWordBag(N, seed);

    std::list<std::pair<std::string,std::string>> LRU;
    std::unordered_map<std::string,
            std::list<std::pair<std::string,std::string>>::iterator> resident;

    out << "lru_profile " << N << " " << seed << "\n";

    for (auto &pr : bag) {
        std::string key = pr.first + " " + pr.second;

        auto it = resident.find(key);

        if (it != resident.end()) {
            LRU.splice(LRU.begin(), LRU, it->second);
            out << "I " << pr.first << " " << pr.second << "\n";
        }
        else if (resident.size() < N) {
            LRU.push_front(pr);
            resident[key] = LRU.begin();
            out << "I " << pr.first << " " << pr.second << "\n";
        }
        else {
            auto victim = LRU.back();
            std::string vKey = victim.first + " " + victim.second;
            resident.erase(vKey);
            LRU.pop_back();

            out << "E " << victim.first << " " << victim.second << "\n";

            LRU.push_front(pr);
            resident[key] = LRU.begin();
            out << "I " << pr.first << " " << pr.second << "\n";
        }
    }

    std::cout << "Generated: " << outName << "\n";
    return 0;
}
