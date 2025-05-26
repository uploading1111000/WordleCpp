#include "optimiser.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

Optimiser::Optimiser(const std::string& WordlistPath, const std::string& FrequenciesPath)
    : wordlist(WordlistPath)
    , colourMatrix([&]() {
        std::cout << "Loaded " << wordlist.size() << " words\n";
        auto matrix = ColourMatrix(wordlist);
        std::cout << "Loaded colour matrix\n";
        return matrix;
    }())
    , indexMatrix([&]() {
        const auto initial = std::chrono::steady_clock::now();
        auto matrix = IndexMatrix(colourMatrix);
        const auto final = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(final - initial);
        std::cout << "Loaded index matrix in " << duration.count() << "ms\n";
        return matrix;
    }())
    , frequencies([&]() {
        auto freq = Frequencies(wordlist, FrequenciesPath);
        std::cout << "Loaded frequencies\n";
        return freq;
    }())
{
    for (int index = 0; index < 243; index++) {
        ALL_COLOURS[index].set(0, index % 3);
        ALL_COLOURS[index].set(1, (index / 3) % 3);
        ALL_COLOURS[index].set(2, (index / 9) % 3);
        ALL_COLOURS[index].set(3, (index / 27) % 3);
        ALL_COLOURS[index].set(4, (index / 81) % 3);
    }
}


void Optimiser::maximiseEntropy2Word()
{
    struct min {
        int first;
        int second;
        float entropy;
    };
    min m = min({ -1, -1, INFINITY });
    for (int i = 0; i < wordlist.size(); i++){
        min mm = min({ i, -1, INFINITY });
        for (int j = 0; j < wordlist.size(); j++){
            if (j == i) continue;
            float mi = Entropy2Word(i, j);
            if (mi < mm.entropy){
                mm.second = j;
                mm.entropy = mi;
            }
        }
        if (mm.entropy < m.entropy){
            m.first = mm.first;
            m.second = mm.second;
            m.entropy = mm.entropy;
        };
        std::cout << wordToString(wordlist[i]) << "(" << i << "/" << wordlist.size() << ") " << mm.entropy << " " << wordToString(wordlist[mm.second]) << "\n";
    }
    std::cout << "Min entropy: " << wordToString(wordlist[m.first]) << " " << wordToString(wordlist[m.second]) << " " << m.entropy << "\n";
}

float Optimiser::Entropy2Word(int first, int second)
{
    float mi = 0.0f;
    std::vector<int> workingSet;
    std::vector<int> &setA = indexMatrix.getIndexSetRef(first, Colours());
    std::vector<int> &setB = indexMatrix.getIndexSetRef(second, Colours());
    for (auto colour1 : ALL_COLOURS) {
        setA = indexMatrix.getIndexSetRef(first, colour1);
        for (auto colour2 : ALL_COLOURS) {
            setB = indexMatrix.getIndexSetRef(second, colour2);
            workingSet.clear();
            std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(),std::back_inserter(workingSet));
            float p = frequencies.setProbability(workingSet);
            if (p > 0.0f) {
                mi += p * log2(p);
            }
        }
    }
    return mi;
}
