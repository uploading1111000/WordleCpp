#include "optimiser.h"
#include <iostream>
#include <chrono>
#include <cmath>

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
}


void Optimiser::maximiseMutualInformation2Word()
{
    struct max {
        int first;
        int second;
        float mutualInformation;
    };
    max m = max({ -1, -1, -INFINITY });
    for (int i = 0; i < wordlist.size(); i++)
    {
        for (int j = 0; j < wordlist.size(); j++)
        {
            if (j == i) continue;
            float mi = mutualInformation2Word(i, j);
            if (mi > m.mutualInformation)
            {
                m.first = i;
                m.second = j;
                m.mutualInformation = mi;
            }
        }
    }
    std::cout << "Max MI: " << wordToString(wordlist[m.first]) << " " << wordToString(wordlist[m.second]) << " " << m.mutualInformation << "\n";
}

float Optimiser::mutualInformation2Word(int first, int second)
{
    return 0.0f;
}
