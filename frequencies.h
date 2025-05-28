#pragma once
#include "utils.h"
class Frequencies
{
public:
	Frequencies(Wordlist& words, const std::string& filename);
	float getIndexFreq(const int& index) const {return freqs[index];};
	float getWordFreq(const Word& word) const {return freqs[wordlist->getWordIndex(word)];};
	float setProbability(const std::vector<int>& set);
protected:
	Wordlist* wordlist;
	std::vector<float> freqs;
};

