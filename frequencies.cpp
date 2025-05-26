#include "frequencies.h"
#include <fstream>
#include <iostream>
#include <map>

struct wordCmp {
    bool operator()(const Word& lhs, const Word& rhs) const {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
};

Frequencies::Frequencies(Wordlist& words, const std::string& filename)
{
	wordlist = &words;
    std::map<Word, float, wordCmp> wordFreq;

    std::ifstream file(filename, std::ios::binary);

    while (file) {
        std::array<char, 5> word;
        float freq;

        if (!file.read(word.data(), 5)) break;

        if (!file.read(reinterpret_cast<char*>(&freq), sizeof(float))) break;

        wordFreq[word] = freq;
    }
    freqs.resize(wordlist->size());
    for (int index = 0; index < words.size(); index++) {
        freqs[index] = wordFreq[words[index]];
    }
}

float Frequencies::setProbability(const std::vector<int>& set)
{
    float prob = 0.0f;
    for (int index : set) {
        prob += freqs[index];
    }
    return prob;
}
